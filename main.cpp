#include <iostream>
#include <cmath>

#include "entt/entt.hpp"
#include "simulation.cpp"

int main() {
    
    float dt = 0.001f;
    position target{40.0f, 0.0f, 45.0f};
    float velocity_init = 30.0f;

    position start{0.0f, 0.0f, 0.0f};
    float bullet_mass = 0.05f;

    position aim = target;

    std::cout << start.x << ", " << start.y << ", " << start.z <<  std::endl;
    std::cout << aim.x << ", " << aim.y << ", " << aim.z <<  std::endl;

    for(int i = 0; i < MAX_SHOTS; i++) {
        position closest_position = simulate(start, aim, dt, bullet_mass, velocity_init);
        // correct aim based on closest position
        float correction_y = target.y - closest_position.y;
        aim.y += correction_y;
        float min_distance = get_distance(closest_position, target);
        std::cerr << "Shot " << i << " min_distance: " << min_distance << " m" << std::endl;
    }

    std::cerr << "Launch angle: " << get_launch_angle(start, aim)*RADIAN_TO_DEGREE << "°" << std::endl;
}