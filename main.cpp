#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>

class GravitySource
{
    sf::Vector2f pos;
    float strength;
    sf::CircleShape s;

public:
    GravitySource(float pos_x, float pos_y, float strength)
    {
        pos.x = pos_x;
        pos.y = pos_y;
        this->strength = strength;

        s.setPosition(pos);
        s.setFillColor(sf::Color::Yellow);
        s.setRadius(7);
    }

    void Render(sf::RenderWindow& wind)
    {
        wind.draw(s);
    }

    sf::Vector2f GetPosition()
    {
        return pos;
    }

    float GetStrength()
    {
        return strength;
    }
};


class Particle 
{
    sf::Vector2f pos;
    sf::Vector2f vel;
    sf::CircleShape s;

public:
    Particle(float pos_x, float pos_y, float vel_x, float vel_y)
    {
        pos.x = pos_x;
        pos.y = pos_y;
        vel.x = vel_x;
        vel.y = vel_y;

        s.setPosition(pos);
        s.setFillColor(sf::Color::Blue);
        s.setRadius(3);
    }

    void Render(sf::RenderWindow& wind)
    {
        s.setPosition(pos);
        wind.draw(s);
    }

    void SetColor(sf::Color col)
    {
        s.setFillColor(col);
    }

    void UpdatePhysics(GravitySource &gs)
    {
        float distance_x = gs.GetPosition().x - pos.x;
        float distance_y = gs.GetPosition().y - pos.y;

        float distance = sqrt(distance_x * distance_x + distance_y * distance_y);
    
        float inverse_distance = 1.f / distance;

        float normalized_x = inverse_distance * distance_x;
        float normalized_y = inverse_distance * distance_y;

        float inverse_square_dropoff = inverse_distance * inverse_distance;

        float acceleration_x = normalized_x * gs.GetStrength() * inverse_square_dropoff;
        float acceleration_y = normalized_y * gs.GetStrength() * inverse_square_dropoff;

        vel.x += acceleration_x;
        vel.y += acceleration_y;

        pos.x += vel.x;
        pos.y += vel.y;
    }
};


sf::Color mapValToColor(float value) // value є [0; 1]
{
    if (value < 0.0f) value = 0.0;
    if (value > 1.0f) value = 1.0;

    int r = 0, g = 0, b = 0;
    
    /* (1)
    // 0 - 0.5 blue -> green
    // 0.5 - 1 green -> red

    if (value < 0.5f)
    {
        b = 255 * (1.0f - 2 * value);
        g = 255 * 2 * value;        
    }
    else 
    {
        g = 255 * (2.0f - 2 * value);
        r = 255 * (2 * value - 1);
    }
    */

    /*
    // (2)
    // 0 - 1 blue -> green
    b = 255 * (1.0f - value);
    g = 255 * value;
    */

    // (3)
    // 0 - 1 blue -> red
    b = 255 * (1.0f - value);
    r = 255 * value;

    return sf::Color(r, g, b);
}


int main()
{
    sf::RenderWindow window(sf::VideoMode(1600, 1000), "Lets go");
    window.setFramerateLimit(60);

    std::vector<GravitySource> sources;
    sources.push_back(GravitySource(500, 500, 7000));
    sources.push_back(GravitySource(1200, 500, 7000));

    int num_particles = 5000;
    std::vector<Particle> particles;
    for (int i = 0; i < num_particles; i ++)
    {
        particles.push_back(Particle(600, 700, 4, 0.2 + (0.1 / num_particles) * i));
    
        // change colors
        float val = float(i) / float(num_particles);

        sf::Color col = mapValToColor(val);

        particles[i].SetColor(col);
    }

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) 
                window.close();
        }

        window.clear();

        for (int i = 0; i < num_particles; i ++)
        {
            for (int j = 0; j < sources.size(); j ++)
            {
                particles[i].UpdatePhysics(sources[j]);
            }
        }

        for (int i = 0; i < sources.size(); i ++)
        {
            sources[i].Render(window);
        }
        for (int i = 0; i < num_particles; i ++)
        {
            particles[i].Render(window);
        }

        window.display();
    }

    return 0;
}
