#include "raylib.h"
#include <vector>
#include <cstdlib>
#include <ctime>

struct Alien
{
    Rectangle rect;
    bool alive;
};

struct Bullet
{
    Rectangle rect;
    float speed;
};

struct EnemyBullet
{
    Rectangle rect;
    float speed;
};

enum class GameState
{
    Menu,
    Playing,
    GameOver
};

void CreateAliens(std::vector<Alien>& aliens)
{
    aliens.clear();

    const int rows = 5;
    const int columns = 10;

    const float startX = 120.0f;
    const float startY = 100.0f;

    const float alienWidth = 40.0f;
    const float alienHeight = 25.0f;

    const float spacingX = 20.0f;
    const float spacingY = 20.0f;

    for (int row = 0; row < rows; row++)
    {
        for (int column = 0; column < columns; column++)
        {
            Alien alien;

            alien.rect = {
                startX + column * (alienWidth + spacingX),
                startY + row * (alienHeight + spacingY),
                alienWidth,
                alienHeight
            };

            alien.alive = true;

            aliens.push_back(alien);
        }
    }
}

int main()
{
    const int screenWidth = 900;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Space Invaders");

    // ESC больше не закрывает окно автоматически.
    SetExitKey(KEY_NULL);

    SetTargetFPS(60);

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    GameState gameState = GameState::Menu;

    // =========================================================
    // ИГРОК
    // =========================================================

    Rectangle player = {
        screenWidth / 2.0f - 25.0f,
        screenHeight - 60.0f,
        50.0f,
        20.0f
    };

    const float playerSpeed = 400.0f;

    // =========================================================
    // ПУЛИ ИГРОКА
    // =========================================================

    std::vector<Bullet> bullets;

    const float playerBulletSpeed = 600.0f;

    // =========================================================
    // ПУЛИ ПРИШЕЛЬЦЕВ
    // =========================================================

    std::vector<EnemyBullet> enemyBullets;

    const float enemyBulletSpeed = 300.0f;
    const float enemyShootInterval = 1.0f;

    float enemyShootTimer = 0.0f;

    // =========================================================
    // ПРИШЕЛЬЦЫ
    // =========================================================

    std::vector<Alien> aliens;

    float alienSpeed = 80.0f;
    float alienDirection = 1.0f;

    // =========================================================
    // ВОЛНЫ
    // =========================================================

    int wave = 1;

    // =========================================================
    // ОЧКИ
    // =========================================================

    int score = 0;

    // =========================================================
    // ЖИЗНИ
    // =========================================================

    const int maxLives = 3;
    int lives = maxLives;

    // Неуязвимость после попадания
    float invulnerabilityTimer = 0.0f;
    const float invulnerabilityDuration = 1.0f;

    // =========================================================
    // ПЕРЕЗАПУСК ИГРЫ
    // =========================================================

    auto ResetGame = [&]()
    {
        player.x = screenWidth / 2.0f - 25.0f;

        bullets.clear();
        enemyBullets.clear();

        wave = 1;
        score = 0;
        lives = maxLives;

        alienSpeed = 80.0f;
        alienDirection = 1.0f;

        enemyShootTimer = 0.0f;
        invulnerabilityTimer = 0.0f;

        CreateAliens(aliens);

        gameState = GameState::Playing;
    };

    CreateAliens(aliens);

    // =========================================================
    // ГЛАВНЫЙ ЦИКЛ
    // =========================================================

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        // =====================================================
        // ГЛАВНОЕ МЕНЮ
        // =====================================================

        if (gameState == GameState::Menu)
        {
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
            {
                ResetGame();
            }

            BeginDrawing();

            ClearBackground(BLACK);

            // Название игры
            const char* title = "SPACE INVADERS";

            int titleWidth = MeasureText(title, 60);

            DrawText(
                title,
                screenWidth / 2 - titleWidth / 2,
                180,
                60,
                WHITE
            );

            // Кнопка START
            Rectangle startButton = {
                screenWidth / 2.0f - 100.0f,
                300.0f,
                200.0f,
                70.0f
            };

            Vector2 mousePosition = GetMousePosition();

            bool mouseOverButton =
                CheckCollisionPointRec(mousePosition, startButton);

            Color buttonColor =
                mouseOverButton ? LIGHTGRAY : DARKGRAY;

            DrawRectangleRec(
                startButton,
                buttonColor
            );

            const char* startText = "START";

            int startTextWidth =
                MeasureText(startText, 30);

            DrawText(
                startText,
                screenWidth / 2 - startTextWidth / 2,
                320,
                30,
                BLACK
            );

            // Запуск мышкой
            if (mouseOverButton &&
                IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                ResetGame();
            }

            const char* hint = "ENTER / SPACE - START";

            int hintWidth =
                MeasureText(hint, 20);

            DrawText(
                hint,
                screenWidth / 2 - hintWidth / 2,
                420,
                20,
                GRAY
            );

            EndDrawing();

            continue;
        }

        // =====================================================
        // GAME OVER
        // =====================================================

        if (gameState == GameState::GameOver)
        {
            // R — новая игра
            if (IsKeyPressed(KEY_R))
            {
                ResetGame();
            }

            // ESC — главное меню
            if (IsKeyPressed(KEY_ESCAPE))
            {
                gameState = GameState::Menu;
            }

            BeginDrawing();

            ClearBackground(BLACK);

            const char* gameOverText = "GAME OVER";

            int gameOverWidth =
                MeasureText(gameOverText, 60);

            DrawText(
                gameOverText,
                screenWidth / 2 - gameOverWidth / 2,
                180,
                60,
                RED
            );

            DrawText(
                TextFormat("FINAL SCORE: %d", score),
                350,
                280,
                25,
                WHITE
            );

            DrawText(
                "R - RESTART",
                350,
                340,
                20,
                LIGHTGRAY
            );

            DrawText(
                "ESC - MAIN MENU",
                350,
                375,
                20,
                LIGHTGRAY
            );

            EndDrawing();

            continue;
        }

        // =====================================================
        // ИГРА
        // =====================================================

        // -----------------------------------------------------
        // Таймер неуязвимости
        // -----------------------------------------------------

        if (invulnerabilityTimer > 0.0f)
        {
            invulnerabilityTimer -= deltaTime;

            if (invulnerabilityTimer < 0.0f)
            {
                invulnerabilityTimer = 0.0f;
            }
        }

        // -----------------------------------------------------
        // Движение игрока
        // -----------------------------------------------------

        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
        {
            player.x -= playerSpeed * deltaTime;
        }

        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
        {
            player.x += playerSpeed * deltaTime;
        }

        // Ограничение игрока по экрану
        if (player.x < 0)
        {
            player.x = 0;
        }

        if (player.x + player.width > screenWidth)
        {
            player.x = screenWidth - player.width;
        }

        // -----------------------------------------------------
        // Стрельба игрока
        // -----------------------------------------------------

        if (IsKeyPressed(KEY_SPACE))
        {
            Bullet bullet;

            bullet.rect = {
                player.x + player.width / 2.0f - 2.5f,
                player.y - 15.0f,
                5.0f,
                15.0f
            };

            bullet.speed = playerBulletSpeed;

            bullets.push_back(bullet);
        }

        // -----------------------------------------------------
        // Движение пуль игрока
        // -----------------------------------------------------

        for (auto& bullet : bullets)
        {
            bullet.rect.y -=
                bullet.speed * deltaTime;
        }

        // Удаляем пули за экраном
        for (int i = static_cast<int>(bullets.size()) - 1;
             i >= 0;
             i--)
        {
            if (bullets[i].rect.y +
                bullets[i].rect.height < 0)
            {
                bullets.erase(bullets.begin() + i);
            }
        }

        // -----------------------------------------------------
        // Движение пришельцев
        // -----------------------------------------------------

        bool changeDirection = false;

        for (const auto& alien : aliens)
        {
            if (!alien.alive)
                continue;

            if (alien.rect.x <= 20 &&
                alienDirection < 0)
            {
                changeDirection = true;
            }

            if (alien.rect.x + alien.rect.width >=
                    screenWidth - 20 &&
                alienDirection > 0)
            {
                changeDirection = true;
            }
        }

        if (changeDirection)
        {
            alienDirection *= -1.0f;

            for (auto& alien : aliens)
            {
                if (alien.alive)
                {
                    alien.rect.y += 20.0f;
                }
            }
        }

        for (auto& alien : aliens)
        {
            if (alien.alive)
            {
                alien.rect.x +=
                    alienDirection *
                    alienSpeed *
                    deltaTime;
            }
        }

        // -----------------------------------------------------
        // Стрельба пришельцев
        // -----------------------------------------------------

        enemyShootTimer += deltaTime;

        if (enemyShootTimer >= enemyShootInterval)
        {
            enemyShootTimer = 0.0f;

            std::vector<int> aliveAliens;

            for (int i = 0;
                 i < static_cast<int>(aliens.size());
                 i++)
            {
                if (aliens[i].alive)
                {
                    aliveAliens.push_back(i);
                }
            }

            if (!aliveAliens.empty())
            {
                int randomIndex =
                    std::rand() %
                    aliveAliens.size();

                int alienIndex =
                    aliveAliens[randomIndex];

                EnemyBullet enemyBullet;

                enemyBullet.rect = {
                    aliens[alienIndex].rect.x +
                        aliens[alienIndex].rect.width /
                        2.0f - 2.5f,

                    aliens[alienIndex].rect.y +
                        aliens[alienIndex].rect.height,

                    5.0f,
                    15.0f
                };

                enemyBullet.speed =
                    enemyBulletSpeed;

                enemyBullets.push_back(
                    enemyBullet
                );
            }
        }

        // -----------------------------------------------------
        // Движение пуль пришельцев
        // -----------------------------------------------------

        for (auto& bullet : enemyBullets)
        {
            bullet.rect.y +=
                bullet.speed * deltaTime;
        }

        // -----------------------------------------------------
        // Попадание пуль игрока в пришельцев
        // -----------------------------------------------------

        for (auto& bullet : bullets)
        {
            for (auto& alien : aliens)
            {
                if (!alien.alive)
                    continue;

                if (CheckCollisionRecs(
                        bullet.rect,
                        alien.rect))
                {
                    alien.alive = false;

                    bullet.rect.y = -100.0f;

                    score += 10;

                    break;
                }
            }
        }

        // -----------------------------------------------------
        // Попадание пуль пришельцев в игрока
        // -----------------------------------------------------

        if (invulnerabilityTimer <= 0.0f)
        {
            for (int i =
                     static_cast<int>(
                         enemyBullets.size()) - 1;
                 i >= 0;
                 i--)
            {
                if (CheckCollisionRecs(
                        enemyBullets[i].rect,
                        player))
                {
                    enemyBullets.erase(
                        enemyBullets.begin() + i
                    );

                    lives--;

                    invulnerabilityTimer =
                        invulnerabilityDuration;

                    player.x =
                        screenWidth / 2.0f -
                        player.width / 2.0f;

                    if (lives <= 0)
                    {
                        gameState =
                            GameState::GameOver;
                    }

                    break;
                }
            }
        }

        // Удаляем пули пришельцев за экраном
        for (int i =
                 static_cast<int>(
                     enemyBullets.size()) - 1;
             i >= 0;
             i--)
        {
            if (enemyBullets[i].rect.y >
                screenHeight)
            {
                enemyBullets.erase(
                    enemyBullets.begin() + i
                );
            }
        }

        // -----------------------------------------------------
        // Новая волна
        // -----------------------------------------------------

        bool allAliensDestroyed = true;

        for (const auto& alien : aliens)
        {
            if (alien.alive)
            {
                allAliensDestroyed = false;
                break;
            }
        }

        if (allAliensDestroyed)
        {
            wave++;

            alienSpeed *= 1.15f;

            alienDirection = 1.0f;

            enemyBullets.clear();

            enemyShootTimer = 0.0f;

            CreateAliens(aliens);
        }

        // -----------------------------------------------------
        // Пришельцы дошли до игрока
        // -----------------------------------------------------

        for (const auto& alien : aliens)
        {
            if (!alien.alive)
                continue;

            if (alien.rect.y +
                    alien.rect.height >=
                player.y)
            {
                gameState =
                    GameState::GameOver;

                break;
            }
        }

        // =====================================================
        // ОТРИСОВКА ИГРЫ
        // =====================================================

        BeginDrawing();

        ClearBackground(BLACK);

        // -----------------------------------------------------
        // ПРИШЕЛЬЦЫ
        // -----------------------------------------------------

        for (const auto& alien : aliens)
        {
            if (!alien.alive)
                continue;

            // Основное тело пришельца
            DrawRectangleRec(
                alien.rect,
                GREEN
            );

            // Глаза
            DrawRectangle(
                static_cast<int>(alien.rect.x + 8),
                static_cast<int>(alien.rect.y + 7),
                6,
                6,
                BLACK
            );

            DrawRectangle(
                static_cast<int>(alien.rect.x + 26),
                static_cast<int>(alien.rect.y + 7),
                6,
                6,
                BLACK
            );
        }

        // -----------------------------------------------------
        // ИГРОК
        // -----------------------------------------------------

        bool drawPlayer = true;

        // Мигание после попадания
        if (invulnerabilityTimer > 0.0f)
        {
            int blink =
                static_cast<int>(
                    invulnerabilityTimer * 10);

            if (blink % 2 == 0)
            {
                drawPlayer = false;
            }
        }

        if (drawPlayer)
        {
            // Основной корпус игрока
            DrawRectangleRec(
                player,
                WHITE
            );

            // Башенка
            DrawRectangle(
                static_cast<int>(
                    player.x + player.width / 2.0f - 5),
                static_cast<int>(
                    player.y - 10),
                10,
                10,
                WHITE
            );
        }

        // -----------------------------------------------------
        // ПУЛИ ИГРОКА
        // -----------------------------------------------------

        for (const auto& bullet : bullets)
        {
            DrawRectangleRec(
                bullet.rect,
                YELLOW
            );
        }

        // -----------------------------------------------------
        // ПУЛИ ПРИШЕЛЬЦЕВ
        // -----------------------------------------------------

        for (const auto& bullet : enemyBullets)
        {
            DrawRectangleRec(
                bullet.rect,
                RED
            );
        }

        // -----------------------------------------------------
        // ИНТЕРФЕЙС
        // -----------------------------------------------------

        DrawText(
            TextFormat(
                "SCORE: %d",
                score
            ),
            20,
            20,
            20,
            WHITE
        );

        DrawText(
            TextFormat(
                "WAVE: %d",
                wave
            ),
            400,
            20,
            20,
            WHITE
        );

        DrawText(
            TextFormat(
                "LIVES: %d",
                lives
            ),
            760,
            20,
            20,
            WHITE
        );

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
