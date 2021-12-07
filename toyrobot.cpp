#include <iostream>
#include <functional>
#include <regex>
#include <array>

constexpr int WIDTH = 5;
constexpr int HEIGHT = 5;

constexpr int NORTH = 0;
constexpr int WEST = 1;
constexpr int SOUTH = 2;
constexpr int EAST = 3;
constexpr int NR_FACES = 4;

const std::array<std::string, 4> g_face_names ({
    "NORTH",
    "WEST",
    "SOUTH",
    "EAST"
});

const std::array<std::string, 4> g_cmd_names ({
    "MOVE",
    "LEFT",
    "RIGHT",
    "REPORT"
});

class ToyRobot {
private:
	int x, y;
	int face;
public:
	ToyRobot() {
		x = -1;
		y = -1;
		face = -1;
	}
	~ToyRobot() {
	}

	bool place(int x, int y, int face) {
		if(x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT || face < 0 || face >= NR_FACES)
			return false;

		this->x = x;
		this->y = y;
		this->face = face;

		return true;
	}

	bool move() {
	    if(x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
	        return false;
	    }

	    switch(face) {
	    case NORTH:
	        if(y < HEIGHT-1)
	            y++;
	        break;
		case SOUTH:
			if(y > 0)
				y--;
			break;
		case EAST:
			if(x < WIDTH-1)
				x++;
			break;
		case WEST:
			if(x > 0)
				x--;
			break;
		}
		return true;
	}

	bool left() {
		if(x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
			return false;
		}

		face++;
		face %= NR_FACES;
		return true;
	}

	bool right() {
		if(x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
			return false;
		}

		if(!face)
			face += NR_FACES;

		face--;
		return true;
	}

	bool report() {
		if(x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
			return false;
		}

		std::cout << x << ", " << y << ", " << g_face_names[face] << std::endl;
		return true;
	}
};

int main(int argc, char *argv[])
{
	ToyRobot robot;

	int lineno = 0;
	do {
		lineno++;

		std::string input;
		std::getline(std::cin, input);
		if(!std::cin.good())
			break;

		std::smatch m;

		std::string pattern("\\s*PLACE\\s(\\s*[+-]?[0-9]+,)(\\s*[+-]?[0-9]+,)");
		pattern += "\\s*(";
		for(auto name: g_face_names)
			pattern += name + "|";
		pattern.pop_back();
		pattern += ")\\s*";

		if(std::regex_search(input, m, std::regex(pattern))) {
			int x, y, face;
			x = std::stoi(m[1]);
			y = std::stoi(m[2]);
			for(face = 0; face < NR_FACES; face++) {
				if(!m[3].compare(g_face_names[face]))
					break;
			}

			if(x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT || face < 0 || face >= NR_FACES) {
				std::cout << lineno << ": " << "Bad argument: " << input << std::endl;
				continue;
			}

			robot.place(x, y, face);
		} else {
			bool matched = false;

			for(auto i = 0; i < g_cmd_names.size(); i++) {
				if(std::regex_match(input, std::regex("\\s*" + g_cmd_names[i] + "\\s*"))) {
					matched = true;

					std::function<int(ToyRobot&)> actions[] = {
						std::mem_fn(&ToyRobot::move),
						std::mem_fn(&ToyRobot::left),
						std::mem_fn(&ToyRobot::right),
						std::mem_fn(&ToyRobot::report)
					};
					if(!actions[i](robot)) {
						std::cout << lineno << ": " << "Bad argument: " << input << std::endl;
					}

					break;
				}
			}

			if(!matched) {
				std::cout << lineno << ": " << "Unknown command: " << input << std::endl;
			}
		}
	} while(1);
	return 0;
}
