#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>

#include "json/json.hpp"
#include "entt/entt.hpp"
#include "simulation.cpp"


void to_json(nlohmann::json_abi_v3_11_3::json& j, const position& pos)
{
    j = nlohmann::json_abi_v3_11_3::json{pos.x, pos.y, pos.z};
}

int main(int argc, char *argv[]) {

    using json = nlohmann::json;

    if(argc != 3) {
        std::cerr << "Usage: " << argv[0] << " input.json output.json" << std::endl;
        return 1;
    }

    std::ifstream f(argv[1]);
    json input_data = json::parse(f);
    
    float dt = input_data["step"];
    position target{40.0f, 0.0f, 45.0f};
    target.x = input_data["target"][0];
    target.y = input_data["target"][1];
    target.z = input_data["target"][2];
    float velocity_init = input_data["velocity"];

    position start{0.0f, 0.0f, 0.0f};
    start.x = input_data["start"][0];
    start.y = input_data["start"][1];
    start.z = input_data["start"][2];
    float bullet_mass = input_data["mass"];

    position aim = target;

    std::cerr << start.x << ", " << start.y << ", " << start.z <<  std::endl;
    std::cerr << aim.x << ", " << aim.y << ", " << aim.z <<  std::endl;

    json output_data;
    output_data["start"] = {start.x, start.y, start.z};
    output_data["target"] = {target.x, target.y, target.z};
    
    std::vector<std::vector<position>> curves;
    

    for(int i = 0; i < MAX_SHOTS; i++) {
        std::vector<position> curve;
        position closest_position = simulate(start, aim, dt, bullet_mass, velocity_init, &curve);
        curves.push_back(curve);
        // correct aim based on closest position
        float correction_y = target.y - closest_position.y;
        aim.y += correction_y;
        float min_distance = get_distance(closest_position, target);
        std::cerr << "Shot " << i << " min_distance: " << min_distance << " m" << std::endl;
    }

    std::cerr << "Launch angle: " << get_launch_angle(start, aim)*RADIAN_TO_DEGREE << "°" << std::endl;

    output_data["curves"] = curves;
    output_data["angle"] = get_launch_angle(start, aim)*RADIAN_TO_DEGREE;

    std::ofstream o(argv[2]);
    o << output_data.dump(4) << std::endl;
}