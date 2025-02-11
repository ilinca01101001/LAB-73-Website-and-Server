use axum::{
    routing::{get, post},
    extract::{Extension, Json, WebSocketUpgrade, ws::{Message, WebSocket}},
    Router,
    response::IntoResponse,
};
use tower_http::cors::{CorsLayer, AllowOrigin, AllowMethods, AllowHeaders};
use std::{
    sync::{Arc},
    net::SocketAddr,
};
use tokio::{sync::{RwLock, broadcast}};
use serde::{Serialize, Deserialize};
use http::{header::HeaderName, Method};
use tokio::net::TcpListener; // TcpListener for binding

#[derive(Serialize, Deserialize, Clone)]
struct State {
    state: String,
}

#[tokio::main]
async fn main() {
    // Create a shared state object and a WebSocket broadcaster
    let state = Arc::new(RwLock::new(State { state: "false".to_string() }));
    let (tx, _) = broadcast::channel::<String>(10); // Channel for WebSocket messages
    let tx = Arc::new(tx);

    // Configure the CORS policy
    let cors = CorsLayer::new()
        .allow_origin(AllowOrigin::list(vec![
            "https://ilinca01101001.github.io".parse().unwrap(),
            "http://127.0.0.1:5500".parse().unwrap(),  // Add this line to allow this origin
        ]))
        .allow_methods(AllowMethods::list(vec![Method::GET, Method::POST]))
        .allow_headers(AllowHeaders::list(vec![HeaderName::from_static("content-type")]));

    // Create the Axum app
    let app = Router::new()
        .route("/toggle-state", post(toggle_state))
        .route("/state", get(get_state))
        .route("/ws", get(handle_websocket)) // WebSocket endpoint
        .layer(cors)
        .layer(Extension(state.clone()))
        .layer(Extension(tx.clone())); // Pass WebSocket broadcaster

    // Run the server
    let addr = SocketAddr::from(([127, 0, 0, 1], 3000));
    println!("Server running on http://{}", addr);

    // Use `TcpListener` for binding
    let listener = TcpListener::bind(&addr).await.unwrap();

    // Use axum's serve function with the listener
    axum::serve(listener, app.into_make_service())
        .await
        .unwrap();
}

// WebSocket handler
async fn handle_websocket(
    ws: WebSocketUpgrade,
    Extension(tx): Extension<Arc<broadcast::Sender<String>>>,
) -> impl IntoResponse {
    ws.on_upgrade(move |socket| websocket_listener(socket, tx))
}

// WebSocket listener for sending state updates
async fn websocket_listener(mut socket: WebSocket, tx: Arc<broadcast::Sender<String>>) {
    let mut rx = tx.subscribe();

    while let Ok(state) = rx.recv().await {
        if socket.send(Message::Text(state.clone())).await.is_err() {
            break;
        }
    }
}

#[derive(Deserialize, Debug)]
struct ToggleRequest {
    message: String, // Make message optional
}

// Handler to toggle the state and broadcast updates
async fn toggle_state(
    Extension(state): Extension<Arc<RwLock<State>>>,
    Extension(state_tx): Extension<Arc<broadcast::Sender<String>>>, // State broadcast
    Extension(message_tx): Extension<Arc<broadcast::Sender<String>>>, // Message broadcast
    Json(payload): Json<ToggleRequest>,
) -> impl IntoResponse {
    // If the message is "change", toggle the state
    if payload.message == "change" {
        let mut state = state.write().await;
        state.state = if state.state == "false" { "true".to_string() } else { "false".to_string() };

        println!("State toggled: {}", state.state);
        
        // Broadcast only the new state (whether toggled or not)
        let _ = state_tx.send(state.state.clone());

    } else {
        // Print the received message if it's not "change"
        println!("Received message: {}", payload.message);
        let _ = message_tx.send(payload.message.clone()); // Send message separately to the message channel
    }
}

// Handler to fetch the current state
async fn get_state(state: Extension<Arc<RwLock<State>>>) -> Json<State> {
    let state = state.read().await;
    Json(state.clone())
}
