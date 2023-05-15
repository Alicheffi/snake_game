#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>

const int CELL_SIZE = 25;                  // Taille en pixels d'une cellule du jeu
const int GRID_WIDTH = 32;                 // Nombre de cellules en largeur
const int GRID_HEIGHT = 24;                // Nombre de cellules en hauteur

enum Direction                            // Enumération pour représenter les directions possibles du serpent
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
        m_segments.push_back({ GRID_WIDTH / 2, GRID_HEIGHT / 2 });    // Ajoute le segment initial du serpent au milieu de la grille
        m_direction = LEFT;                                           // Initialise la direction du serpent vers la gauche

        srand(static_cast<unsigned int>(time(nullptr)));             // Initialise le générateur de nombres aléatoires avec une graine basée sur l'heure actuelle
        placeFood();                                                  // Place la nourriture initiale de manière aléatoire sur la grille
    }

    void setDirection(Direction direction)
    {
        if (m_direction == UP && direction == DOWN ||                 // Vérifie si la direction demandée est opposée à la direction actuelle
            m_direction == DOWN && direction == UP ||
            m_direction == LEFT && direction == RIGHT ||
            m_direction == RIGHT && direction == LEFT)
        {
            return;                                                   // Si c'est le cas, ne change pas la direction du serpent
        }

        m_direction = direction;                                      // Sinon, met à jour la direction du serpent
    }

    void update()
    {
        sf::Vector2i head = m_segments.front();                       // Obtient la position de la tête du serpent
        switch (m_direction)
        {
        case UP:
            head.y -= 1;                                              // Déplace la tête vers le haut d'une cellule
            break;
        case DOWN:
            head.y += 1;                                              // Déplace la tête vers le bas d'une cellule
            break;
        case LEFT:
            head.x -= 1;                                              // Déplace la tête vers la gauche d'une cellule
            break;
        case RIGHT:
            head.x += 1;                                              // Déplace la tête vers la droite d'une cellule
            break;
        }
        m_segments.insert(m_segments.begin(), head);                   // Insère la nouvelle position de la tête au début du vecteur des segments

        if (head.x == m_foodX && head.y == m_foodY)                    // Vérifie si la tête du serpent atteint la nourriture
        {
            placeFood();                                               // Place une nouvelle nourriture de manière aléatoire
        }
        else
        {
            m_segments.pop_back();                                     // Si le serpent n'a pas mangé de nourriture, supprime le dernier segment pour maintenir sa taille
        }

        if (head.x < 0 || head.x >= GRID_WIDTH ||                      // Vérifie si la tête du serpent sort de la grille ou entre en collision avec lui-même
            head.y < 0 || head.y >= GRID_HEIGHT ||
            isCollidingWithSelf())
        {
            reset();                                                   // Si c'est le cas, réinitialise le serpent
        }
    }

    const std::vector<sf::Vector2i>& getSegments() const
    {
        return m_segments;                                             // Retourne une référence constante au vecteur des segments du serpent
    }

    int getFoodX() const
    {
        return m_foodX;                                              // Retourne la position en X de la nourriture
    }

    int getFoodY() const
    {
        return m_foodY;                                              // Retourne la position en Y de la nourriture
    }

private:
    void placeFood()
    {
        do
        {
            m_foodX = rand() % GRID_WIDTH;                            // Place la nourriture de manière aléatoire en X sur la grille
            m_foodY = rand() % GRID_HEIGHT;                           // Place la nourriture de manière aléatoire en Y sur la grille
        } while (isCollidingWithSelf() || (m_foodX == m_segments.front().x && m_foodY == m_segments.front().y));   // Vérifie que la nourriture n'est pas en collision avec le serpent ou qu'elle n'est pas à la même position que la tête du serpent
    }

    bool isCollidingWithSelf() const
    {
        for (auto iter = m_segments.begin() + 1; iter != m_segments.end(); ++iter)    // Parcourt tous les segments du serpent à partir du deuxième
        {
            if (iter->x == m_segments.front().x && iter->y == m_segments.front().y)    // Vérifie si le segment est en collision avec la tête du serpent
            {
                return true;                                                           // Retourne vrai si une collision est détectée
            }
        }
        return false;                                                                  // Retourne faux s'il n'y a pas de collision avec le serpent lui-même
    }

    void reset()
    {
        m_segments.clear();                                            // Supprime tous les segments du serpent
        m_segments.push_back({ GRID_WIDTH / 2, GRID_HEIGHT / 2 });    // Ajoute un nouveau segment initial au milieu de la grille
        m_direction = LEFT;                                           // Réinitialise la direction du serpent vers la gauche
        placeFood();                                                   // Place une nouvelle nourriture de manière aléatoire
    }

    std::vector<sf::Vector2i> m_segments;                              // Vecteur des segments du serpent
    Direction m_direction;                                             // Direction actuelle du serpent
    int m_foodX;                                                       // Position en X de la nourriture
    int m_foodY;                                                       // Position en Y de la nourriture
};

int main()
{
    sf::RenderWindow window(sf::VideoMode(GRID_WIDTH * CELL_SIZE, GRID_HEIGHT * CELL_SIZE), "Snake");   // Crée la fenêtre du jeu avec la taille en pixels calculée en fonction de la grille

    Snake snake;                                                                                      // Crée un objet Snake
    sf::Font font;
    if (!font.loadFromFile("arial.ttf"))
    {
        return EXIT_FAILURE;
    }
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);

    sf::RectangleShape cellBackground(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));                    // Crée un rectangle représentant une cellule du jeu
    cellBackground.setFillColor(sf::Color::Red);
    cellBackground.setOutlineThickness(1);
    cellBackground.setOutlineColor(sf::Color::Black);

    sf::Texture foodTexture;
    if (!foodTexture.loadFromFile("alii.png")) {
    }
    sf::Sprite foodSprite(foodTexture);
    foodSprite.setScale(CELL_SIZE / foodTexture.getSize().x, CELL_SIZE / foodTexture.getSize().y);

    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("background.png"))
    {
        return EXIT_FAILURE;
    }
    sf::Sprite backgroundSprite(backgroundTexture);
    backgroundSprite.setScale((float)window.getSize().x / backgroundTexture.getSize().x, (float)window.getSize().y / backgroundTexture.getSize().y);
    sf::Clock clock;
    sf::Music music;
    if (!music.openFromFile("chiptune-grooving-142242.ogg"))
    {
        // Gestion d'erreur si le chargement de la musique échoue
    }
    music.play();


    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();  // Ferme la fenêtre si l'événement de fermeture est détecté
                break;
            case sf::Event::KeyPressed:
                switch (event.key.code)
                {
                case sf::Keyboard::Up:
                    snake.setDirection(UP);     // Met à jour la direction du serpent vers le haut si la touche flèche haut est enfoncée
                    break;
                case sf::Keyboard::Down:
                    snake.setDirection(DOWN);   // Met à jour la direction du serpent vers le bas si la touche flèche bas est enfoncée
                    break;
                case sf::Keyboard::Left:
                    snake.setDirection(LEFT);   // Met à jour la direction du serpent vers la gauche si la touche flèche gauche est enfoncée
                    break;
                case sf::Keyboard::Right:
                    snake.setDirection(RIGHT);  // Met à jour la direction du serpent vers la droite si la touche flèche droite est enfoncée
                    break;
                }
                break;
            }
        }

        if (clock.getElapsedTime().asMilliseconds() >= 100)
        {
            snake.update();         // Met à jour la position du serpent à intervalles réguliers
            clock.restart();        // Redémarre l'horloge pour suivre le temps écoulé depuis la dernière mise à jour du serpent
        }

        window.clear();
        window.draw(backgroundSprite);          // Dessine l'arrière-plan du jeu

        for (auto segment : snake.getSegments())
        {
            cellBackground.setPosition(segment.x * CELL_SIZE, segment.y * CELL_SIZE);
            window.draw(cellBackground);        // Dessine chaque segment du serpent
        }
        foodSprite.setPosition(snake.getFoodX() * CELL_SIZE, snake.getFoodY() * CELL_SIZE);
        window.draw(foodSprite);                 // Dessine la nourriture du serpent
        scoreText.setString("Score: " + std::to_string(snake.getSegments().size() - 1));
        window.draw(scoreText);                   // Dessine le texte du score du joueur
        window.display();                         // Affiche tout ce qui a été dessiné à l'écran
    }
    music.stop();                                 // Arrête la lecture de la musique
    return EXIT_SUCCESS;
}

