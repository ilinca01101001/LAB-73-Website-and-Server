use axum::{
    routing::{get, post},
    Router,
    extract::{Extension, Json},
};
use tower_http::cors::{CorsLayer, AllowOrigin, AllowMethods, AllowHeaders};
use std::sync::Arc;
use tokio::sync::RwLock;
use serde::{Serialize, Deserialize};
use std::net::SocketAddr;
use http::{header::HeaderName, Method};

#[derive(Serialize, Deserialize, Clone)]
struct State {
    state: String,
}

#[tokio::main]
async fn main() {
    // Create a shared state object
    let state = Arc::new(RwLock::new(State {
        state: "false".to_string(),
    }));

    // Configure the CORS policy
    let cors = CorsLayer::new()
        .allow_origin(AllowOrigin::list(vec!["https://ilinca01101001.github.io".parse().unwrap()])) // Use `list` for multiple origins
        .allow_methods(AllowMethods::list(vec![Method::GET, Method::POST])) // Use `list` for methods
        .allow_headers(AllowHeaders::list(vec![HeaderName::from_static("content-type")])); // Use `list` for headers

    // Create the Axum app with routes and CORS middleware
    let app = Router::new()
        .route("/toggle-state", post(toggle_state))
        .route("/state", get(get_state))
        .layer(cors)  // Add the CORS middleware to the app
        .layer(Extension(state.clone()));  // Pass shared state using `Extension`

    // Run the server
    let addr = SocketAddr::from(([127, 0, 0, 1], 3000));
    println!("Server running on http://{}", addr);
    axum::Server::bind(&addr)
        .serve(app.into_make_service())
        .await
        .unwrap();
}

// Handler to toggle the state
async fn toggle_state(state: Extension<Arc<RwLock<State>>>) {
    let mut state = state.write().await;
    if state.state == "false" {
        state.state = "true".to_string();
    } else {
        state.state = "false".to_string();
    }
    println!("State toggled: {}", state.state); // Logs the state change
}

// Handler to fetch the current state
async fn get_state(state: Extension<Arc<RwLock<State>>>) -> Json<State> {
    let state = state.read().await;
    Json(state.clone()) // Return the state as a JSON response
}
