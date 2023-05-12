#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>

const int CELL_SIZE = 25;
const int GRID_WIDTH = 32;
const int GRID_HEIGHT = 24;

enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class Snake
{
public:
    Snake()
    {
        m_segments.push_back({ GRID_WIDTH / 2, GRID_HEIGHT / 2 });
        m_direction = LEFT;

        srand(static_cast<unsigned int>(time(nullptr)));
        placeFood();
    }

    void setDirection(Direction direction)
    {
        if (m_direction == UP && direction == DOWN ||
            m_direction == DOWN && direction == UP ||
            m_direction == LEFT && direction == RIGHT ||
            m_direction == RIGHT && direction == LEFT)
        {
            return;
        }

        m_direction = direction;
    }

    void update()
    {
        sf::Vector2i head = m_segments.front();
        switch (m_direction)
        {
        case UP:
            head.y -= 1;
            break;
        case DOWN:
            head.y += 1;
            break;
        case LEFT:
            head.x -= 1;
            break;
        case RIGHT:
            head.x += 1;
            break;
        }
        m_segments.insert(m_segments.begin(), head);

        if (head.x == m_foodX && head.y == m_foodY)
        {
            placeFood();
        }
        else
        {
            m_segments.pop_back();
        }

        if (head.x < 0 || head.x >= GRID_WIDTH ||
            head.y < 0 || head.y >= GRID_HEIGHT ||
            isCollidingWithSelf())
        {
            reset();
        }
    }

    const std::vector<sf::Vector2i>& getSegments() const
    {
        return m_segments;
    }

    int getFoodX() const
    {
        return m_foodX;
    }

    int getFoodY() const
    {
        return m_foodY;
    }

private:
    void placeFood()
    {
        do
        {
            m_foodX = rand() % GRID_WIDTH;
            m_foodY = rand() % GRID_HEIGHT;
        } while (isCollidingWithSelf() || (m_foodX == m_segments.front().x && m_foodY == m_segments.front().y));
    }

    bool isCollidingWithSelf() const
    {
        for (auto iter = m_segments.begin() + 1; iter != m_segments.end(); ++iter)
        {
            if (iter->x == m_segments.front().x && iter->y == m_segments.front().y)
            {
                return true;
            }
        }
        return false;
    }

    void reset()
    {
        m_segments.clear();
        m_segments.push_back({ GRID_WIDTH / 2, GRID_HEIGHT / 2 });
        m_direction = LEFT;
        placeFood();
    }

    std::vector<sf::Vector2i> m_segments;
    Direction m_direction;
    int m_foodX;
    int m_foodY;
};

int main()
{
    sf::RenderWindow window(sf::VideoMode(GRID_WIDTH * CELL_SIZE, GRID_HEIGHT * CELL_SIZE), "Snake");

    Snake snake;
    sf::Font font;
    if (!font.loadFromFile("arial.ttf"))
    {
        return EXIT_FAILURE;
    }
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);

    sf::RectangleShape gridBackground(sf::Vector2f(CELL_SIZE * GRID_WIDTH, CELL_SIZE * GRID_HEIGHT));
    gridBackground.setFillColor(sf::Color::Black);

    sf::RectangleShape cellBackground(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));
    cellBackground.setFillColor(sf::Color::Red);
    cellBackground.setOutlineThickness(1);
    cellBackground.setOutlineColor(sf::Color::Black);

    sf::Texture foodTexture;
    if (!foodTexture.loadFromFile("alii.png")) {
    }
    sf::Sprite foodSprite(foodTexture);
    foodSprite.setScale(CELL_SIZE / foodTexture.getSize().x, CELL_SIZE / foodTexture.getSize().y);


    sf::Clock clock;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::KeyPressed:
                switch (event.key.code)
                {
                case sf::Keyboard::Up:
                    snake.setDirection(UP);
                    break;
                case sf::Keyboard::Down:
                    snake.setDirection(DOWN);
                    break;
                case sf::Keyboard::Left:
                    snake.setDirection(LEFT);
                    break;
                case sf::Keyboard::Right:
                    snake.setDirection(RIGHT);
                    break;
                }
                break;
            }
        }

        if (clock.getElapsedTime().asMilliseconds() >= 100)
        {
            snake.update();
            clock.restart();
        }

        window.clear();
        window.draw(gridBackground);

        for (auto segment : snake.getSegments())
        {
            cellBackground.setPosition(segment.x * CELL_SIZE, segment.y * CELL_SIZE);
            window.draw(cellBackground);
        }
        foodSprite.setPosition(snake.getFoodX() * CELL_SIZE, snake.getFoodY() * CELL_SIZE);
        window.draw(foodSprite);
        scoreText.setString("Score: " + std::to_string(snake.getSegments().size() - 1));
        window.draw(scoreText);
        window.display();
    }

    return EXIT_SUCCESS;
}

