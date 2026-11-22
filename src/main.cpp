#include "raylib.h"
#include <cmath>

// Reset the ball to the middle
void ResetBall(Vector2 &ball, Vector2 &ballSpeed, int round) {

    ball.x = 400;
    ball.y = 300;

    if (round % 3 == 0) {
        ballSpeed.x = -270;
        ballSpeed.y = -100;
    }
    else if (round % 3 == 1) {
        ballSpeed.x = 270;
        ballSpeed.y = 100;
    }
    else {
        ballSpeed.x = 60;
        ballSpeed.y = -270;
    }
}

// Make the ball faster after every paddle hit
void SpeedUpBall(Vector2 &ballSpeed) {

    float speedIncrease = 1.05f; // f is appended to the value to force C++ to immediately save the value as float, as opposed to a double that is converted to a float
    float maxSpeed = 650.0f;

    // Increase speed by 5%
    ballSpeed.x *= speedIncrease;
    ballSpeed.y *= speedIncrease;

    // Find the current ball speed
    float currentSpeed = std::sqrt(
        ballSpeed.x * ballSpeed.x +
        ballSpeed.y * ballSpeed.y
    );

    // Keep the ball from getting too fast
    if (currentSpeed > maxSpeed) {

        ballSpeed.x = (ballSpeed.x / currentSpeed) * maxSpeed;
        ballSpeed.y = (ballSpeed.y / currentSpeed) * maxSpeed;
    }
}

int main() {

    // Window settings
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "3 Player Pong"); // Sets up window dimensions, and window label.
    SetTargetFPS(60);   // Sets max fps to 60.

    // Create three paddles
    Rectangle player1 = {30, 250, 15, 100}; // Initializes a simple rectangle with parameters {x pos, y pos, width, height}
    Rectangle player2 = {755, 250, 15, 100};    
    Rectangle player3 = {340, 65, 120, 15};

    float paddleSpeed = 350;

    // Create the ball
    Vector2 ball = {400, 300}; // Creates a 2D vector object with parameters {x, y}. Located in center of window.
    Vector2 ballSpeed = {-270, -100}; // Creates a 2D vector object with the x & y values as base ball speed

    float ballRadius = 10;

    // Player scores
    int score1 = 0;
    int score2 = 0;
    int score3 = 0;

    // Game settings
    const int winningScore = 5;

    int winner = 0;
    int lastHit = 0;
    int round = 0;

    bool paused = false;

    while (!WindowShouldClose()) {  // "Window" loop for game, if escape key hasn't been pressed, the loop continues to run. Once escape key is pressed, the loop ends, and the closeWindow() function is called to terminate the window.

        float deltaTime = GetFrameTime(); // Sets value of deltaTime to amount of time since last frame drawn

        // Pause or resume the game
        if (IsKeyPressed(KEY_SPACE) && winner == 0) { // If space has been pressed & win conditions haven't been met, pause state is set to the opposite of it's current state
            paused = !paused;
        }

        // Restart the game
        if (IsKeyPressed(KEY_R)) { // If the R key is pressed at anytime, all conditions relating to score, position of ball, position of paddles, etc... are reset to default values

            score1 = 0;
            score2 = 0;
            score3 = 0;

            winner = 0;
            lastHit = 0;
            round = 0;
            paused = false;

            player1.y = 250;
            player2.y = 250;
            player3.x = 340;

            ResetBall(ball, ballSpeed, round);
        }

        // Only update the game while playing
        if (!paused && winner == 0) {

            // Player 1 controls (W and S)
            if (IsKeyDown(KEY_W)) { // Checks if key is held down as opposed to pressed
                player1.y -= paddleSpeed * deltaTime; // Changes x or y value of player rectangle location by amount equal to product of paddleSpeed & deltaTime
            }

            if (IsKeyDown(KEY_S)) {
                player1.y += paddleSpeed * deltaTime;
            }

            // Player 2 controls (Arrow keys)
            if (IsKeyDown(KEY_UP)) {
                player2.y -= paddleSpeed * deltaTime;
            }

            if (IsKeyDown(KEY_DOWN)) {
                player2.y += paddleSpeed * deltaTime;
            }

            // Player 3 controls (Y and H)
            if (IsKeyDown(KEY_Y)) {
                player3.x -= paddleSpeed * deltaTime;
            }

            if (IsKeyDown(KEY_H)) {
                player3.x += paddleSpeed * deltaTime;
            }

            // Keep Player 1 inside the screen
            if (player1.y < 0) { // Checks the x or y value of player rectangle, and if it's outside bounds set it resets player position to the edge the passed bound
                player1.y = 0;
            }

            if (player1.y > screenHeight - player1.height) {
                player1.y = screenHeight - player1.height;
            }

            // Keep Player 2 inside the screen
            if (player2.y < 0) {
                player2.y = 0;
            }

            if (player2.y > screenHeight - player2.height) {
                player2.y = screenHeight - player2.height;
            }

            // Keep Player 3 inside the screen
            if (player3.x < 0) {
                player3.x = 0;
            }

            if (player3.x > screenWidth - player3.width) {
                player3.x = screenWidth - player3.width;
            }

            // Move the ball
            ball.x += ballSpeed.x * deltaTime; // Increments position of the ball by pos/neg values, equal to the product of the x/y value of ballSpeed and deltaTime
            ball.y += ballSpeed.y * deltaTime;

            // Bounce off the bottom wall
            if (ball.y + ballRadius >= screenHeight) { // The radius of the ball plus the x or y position of the ball is used as opposed to just the x or y position to prevent the ball from getting stuck in a collision loop with the bounds of the screen

                ball.y = screenHeight - ballRadius;
                ballSpeed.y = -std::fabs(ballSpeed.y); // The fabs() function returns the absolute value of a floating point number
            }

            // Collision with Player 1
            if (ballSpeed.x < 0 && CheckCollisionCircleRec(ball, ballRadius, player1)) { // Checks if ballspeed perpindicular to the player rectangle is above zero and the collision conditions have been met

                ball.x = player1.x + player1.width + ballRadius; // Sets x position of ball to the edge of player rectangle + ball radius to prevent infinite collision

                ballSpeed.x = std::fabs(ballSpeed.x);

                // Change the ball's angle based on where it hits
                float hitPosition =
                    ball.y - (player1.y + player1.height / 2);

                ballSpeed.y += hitPosition * 2;

                // Make the ball faster
                SpeedUpBall(ballSpeed);

                lastHit = 1; // The lastHit variable is assigned a value corresponding to the player who last touched it
            }

            // Collision with Player 2
            if (ballSpeed.x > 0 && CheckCollisionCircleRec(ball, ballRadius, player2)) {

                ball.x = player2.x - ballRadius;

                ballSpeed.x = -std::fabs(ballSpeed.x);

                float hitPosition =
                    ball.y - (player2.y + player2.height / 2);

                ballSpeed.y += hitPosition * 2;

                // Make the ball faster
                SpeedUpBall(ballSpeed);

                lastHit = 2;
            }

            // Collision with Player 3
            if (ballSpeed.y < 0 && CheckCollisionCircleRec(ball, ballRadius, player3)) {

                ball.y = player3.y + player3.height + ballRadius;

                ballSpeed.y = std::fabs(ballSpeed.y);

                float hitPosition =
                    ball.x - (player3.x + player3.width / 2);

                ballSpeed.x += hitPosition * 2;

                // Keep the ball moving horizontally
                if (ballSpeed.x > -90 && ballSpeed.x < 90) {

                    if (ballSpeed.x >= 0) {
                        ballSpeed.x = 90;
                    }
                    else {
                        ballSpeed.x = -90;
                    }
                }

                // Make the ball faster
                SpeedUpBall(ballSpeed);

                lastHit = 3;
            }

            // Check if a player missed the ball
            int missedPlayer = 0;

            if (ball.x + ballRadius < 0) { // If the ball passes the bounds of an edge of the screen, the missedPlayer variable is assigned a number corresponding player whose edge of the screen was crossed
                missedPlayer = 1;
            }
            else if (ball.x - ballRadius > screenWidth) {
                missedPlayer = 2;
            }
            else if (ball.y + ballRadius < 0) {
                missedPlayer = 3;
            }

            // Award points when someone misses
            if (missedPlayer != 0) {

                int pointTo = lastHit;

                // If nobody hit the ball yet,
                // give the point to another player
                if (pointTo == 0 || pointTo == missedPlayer) {

                    if (missedPlayer == 1) {
                        pointTo = 2;
                    }
                    else if (missedPlayer == 2) {
                        pointTo = 1;
                    }
                    else {
                        pointTo = (ball.x < 400) ? 1 : 2;
                    }
                }

                if (pointTo == 1) {
                    score1++;
                }
                else if (pointTo == 2) {
                    score2++;
                }
                else if (pointTo == 3) {
                    score3++;
                }

                // Check for a winner
                if (score1 >= winningScore) {
                    winner = 1;
                }
                else if (score2 >= winningScore) {
                    winner = 2;
                }
                else if (score3 >= winningScore) {
                    winner = 3;
                }

                // Start the next round
                if (winner == 0) {

                    round++;
                    lastHit = 0;

                    ResetBall(ball, ballSpeed, round);
                }
            }
        }

        // Draw the game
        BeginDrawing(); // Begins drawing all objects onto screen, i.e. creates the "frame"

        ClearBackground(BLACK); // Clears the screen at the beginning of each frame so that each frame is drawn over a blank canvas

        // Draw the center line
        for (int y = 150; y < screenHeight; y += 35) {
            DrawRectangle(398, y, 4, 18, DARKGRAY);
        }

        // Draw the scoreboard
        DrawText(
            TextFormat("P1: %d", score1),
            30, 15, 25, SKYBLUE
        );

        DrawText(
            TextFormat("P3: %d", score3),
            350, 15, 25, GREEN
        );

        DrawText(
            TextFormat("P2: %d", score2),
            680, 15, 25, ORANGE
        );

        // Game title
        DrawText("3 PLAYER PONG", 300, 115, 28, WHITE);

        // Draw Player 1 paddle
        DrawRectangleRec(player1, SKYBLUE);

        // Draw Player 2 paddle
        DrawRectangleRec(player2, ORANGE);

        // Draw Player 3 paddle
        DrawRectangleRec(player3, GREEN);

        // Draw the ball
        DrawCircleV(ball, ballRadius, WHITE);

        // Show pause message
        if (paused && winner == 0) {

            DrawRectangle(230, 250, 340, 100, BLACK);

            DrawText("PAUSED", 315, 260, 35, YELLOW);

            DrawText(
                "Press SPACE to resume",
                275, 315, 20, WHITE
            );
        }

        // Show winner message
        if (winner != 0) {

            DrawRectangle(180, 225, 440, 170, BLACK);

            DrawText(
                TextFormat("PLAYER %d WINS!", winner),
                240, 255, 35, YELLOW
            );

            DrawText(
                "Press R to play again",
                270, 325, 22, WHITE
            );
        }

        // Controls at the bottom
        DrawText(
            "P1: W/S     P2: UP/DOWN     P3: Y/H",
            175, 565, 17, GRAY
        );

        EndDrawing(); // Ends drawing of objects, i.e. the completes the building of the frame
    }

    CloseWindow();

    return 0;
}