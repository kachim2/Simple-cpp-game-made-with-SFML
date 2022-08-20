#include <SFML/Graphics.hpp> // gra używa biblioteki SFML ze strony https://www.sfml-dev.org/
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Network.hpp>
#include <mutex>
#include <thread>
#include <fstream>
#include <iostream>
#include <vector>
#include "SHA256.h"
bool dot = 0;
using namespace sf;
bool erroro = false;
Texture* killer_txt = new Texture[1];
Texture middle;
Texture player_idle;
Texture white;
Texture treasure_txt;
Texture player_txt1;
Texture player_txt2;
Texture jump_txt;
Texture endplat;
Texture doors_txt;
Vector2i mapsize = Vector2i(1280, 720);
sf::View view;
RenderWindow window{ VideoMode(1280,720), "C++ Game" };
Sprite player(player_txt1, IntRect(0,0,50,50));
std::vector<Sprite>otherplayers;
Sprite ground;
FloatRect next_position;
Vector2f velocity;
Vector2f Positionfornet;
Text You_Won;
Font font;
int level = 1;
int killernumber = 0;
int platforms_number = 4;
bool playertxt = 0;
bool jump_able;
bool stop = 0;
uint8_t mside = 0;
SHA256 msha;
uint8_t * digestp;
uint8_t mhash[32];
std::mutex msm;
std::mutex mhashm;
std::mutex opu;
std::mutex possm;
bool Networking(){
	TcpSocket socket;
	//std::cerr << 1;
	Socket::Status status = socket.connect("betterweb.gq", 5300);
	
	if (status != Socket::Done){
		
		return 0;
	}
	Int32 ammountofplayers, Xpos, Ypos;
	while (1){
	Packet Spacket, Rpacket;
	possm.lock();
	Xpos = Positionfornet.x;
	Ypos = Positionfornet.y;
	possm.unlock();
	mhashm.lock();
	msm.lock();
	Spacket << Xpos << Ypos << mside;
	for(int i = 0; i < 32; i++){
		Spacket << mhash[i];
	}
	msm.unlock();
	mhashm.unlock();
	
	socket.send(Spacket);
	socket.receive(Rpacket);
	Rpacket >> ammountofplayers;
	std::vector<Int32> opsX(ammountofplayers);
	std::vector<Int32> opsY(ammountofplayers);
	std::vector<uint8_t> side(ammountofplayers);
	//std::cerr << ammountofplayers;

	for(int i = 0; i < ammountofplayers; i++){
		Rpacket >> opsX[i];
		Rpacket >> opsY[i];
		Rpacket >> side[i];
	}
	
	for(int i = 0; i < ammountofplayers; i++){
		if(otherplayers.size() <= i)
		otherplayers.push_back(Sprite(player_txt1, IntRect(0,0,50,50)));
		otherplayers[i].setPosition(Vector2f(opsX[i],opsY[i]));
		if(side[i] > 3){
		otherplayers[i].setScale(-1, 1);
		side[i]-=4;
		}
		else{
		otherplayers[i].setScale(1, 1);
		}
		//std::cerr << (int)side[i];
		switch (side[i])
		{
		case 0:
			otherplayers[i].setTexture(player_txt1);
			break;
		case 1:
			otherplayers[i].setTexture(player_txt2);
			break;
		case 2:
			otherplayers[i].setTexture(player_idle);
			break;
		case 3:
			otherplayers[i].setTexture(jump_txt);
			break;
		default:
			break;
		}
	}
	for(int i = otherplayers.size(); i >ammountofplayers; i--){
		otherplayers.pop_back();
	}
	if (stop) {
		socket.disconnect();
		return 0;
	}
	}
	return 0;
}
std::string readFileIntoString(const std::string& path) {
	std::ifstream input_file(path);
	if (!input_file.is_open()) {
		std::cerr << "Could not open the file - '"
			<< path << "'" << std::endl;
		exit(EXIT_FAILURE);
	}
	return std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
}
class Obstacle
{

public:
	Sprite shape;
	Vector2f Size, x_y;
	Obstacle(Texture txt = white,Vector2f Start_Size = Vector2f(0,0), Vector2f Start_x_y = Vector2f(0,0)) {
		white.setRepeated(true);
		Size = Start_Size;
		x_y = Start_x_y;
		shape.setTexture(txt);
		
		update();
	}
	void update() {
		
		shape.setPosition(x_y);
		shape.setScale(1, 1);
		shape.setTextureRect(IntRect(0, 0, Size.x, Size.y));
	}
	void draw(){
		window.draw(shape);


	}

	
};
Obstacle* treasure;
Obstacle* doors = new Obstacle[1];
Obstacle* killer = new Obstacle[1];

class Platform
{
public:
	Obstacle end;
	Obstacle middlee;
	Vector2f Size;
	Vector2f Position;
	RectangleShape shape{ Size };
	
	Platform() {
		
		middle.loadFromFile("assets/dirt.png");
		endplat.loadFromFile("assets/dirtkoniec.png");
		end.shape.setTexture(endplat);
		middle.setRepeated(true);
		middlee.shape.setTexture(middle);
		shape.setPosition(Position);
		shape.setSize(Size);

	}
	void draw(){
		
		end.x_y = Position;
		end.Size = Vector2f(40, Size.y);
		end.update();
		end.shape.setScale(1, 1);
		end.draw();
		middlee.x_y = Vector2f(Position.x + 40, Position.y);
		middlee.Size = Vector2f(Size.x - 80, Size.y);
		middlee.update();
		middlee.draw();
		end.x_y = Vector2f(Position.x + Size.x, Position.y);
		end.Size = Vector2f(40, Size.y);
		end.update();
		end.shape.setScale(-1, 1);
		end.draw();

	}

};
Platform* platforms = new Platform[4];
int colide() {
	int ret = -1;
	FloatRect Player_Bounds = player.getGlobalBounds();
	next_position.left = Player_Bounds.left + velocity.x;
	next_position.top = Player_Bounds.top + velocity.y;
	for (int j = 0; j < platforms_number ; j++) {
		FloatRect ObstacleBounds;
		ObstacleBounds.left = platforms[j].Position.x;
		ObstacleBounds.top = platforms[j].Position.y;
		ObstacleBounds.height = platforms[j].Size.y;
		ObstacleBounds.width = platforms[j].Size.x;
		if (ObstacleBounds.intersects(next_position))
		{
			ret = j;

		}


	}
	for (int j = 0; j < otherplayers.size(); j++) {
		FloatRect ObstacleBounds = otherplayers[j].getGlobalBounds();
		if (ObstacleBounds.intersects(next_position))
		{
			if (otherplayers[j].getPosition().y - player.getPosition().y > 30) {
				player.setPosition(40, 400);
			}
			ret = j;

		}


	}
	return ret + 1;


}


void mapf_init(std::string &mapfilename){

	std::fstream mapf;
	mhashm.lock();
	std::string s = readFileIntoString(mapfilename);
	msha.update(s);
	digestp = msha.digest();
	mhashm.unlock();
	for (int i = 0; i < 32; i++) {
		mhash[i] = digestp[i];
	}
	
	mapf.open(mapfilename, std::ios_base::in);
	if(!mapf.is_open()){
		std::cerr << "file is not opened";
		Clock errorclock;
		while (errorclock.getElapsedTime().asSeconds() < 5);
		erroro = true;
		return;
	}
	std::string arg;
	player.setPosition(40, 400);
	while (mapf.good())
	{
		int val = 0;
		mapf >> arg;
		if(arg == "killern"){
			mapf >> val;
			killernumber = val;
			delete[] killer;
			killer = new Obstacle[val]{killer_txt[0]};
			for (int i = 0; i < val; i++)
			{
				killer[i].shape.setTexture(killer_txt[0]);
			}
			
			

		}else if(arg == "platformn"){
			mapf>> val;
			platforms_number = val;
			delete[] platforms;
			platforms = new Platform[val];
		}else if(arg == "killer"){
			mapf >> val;
			std::string arg2;
			mapf >> arg2;
			if(arg2 == "size"){
				int val2, val3{0};
				mapf >> val2 >> val3;
				killer[val].Size = Vector2f(val2, val3);
			}
			mapf >> arg2;
			if(arg2 == "pos"){
				int val2, val3{0};
				mapf >> val2 >> val3;
				killer[val].x_y = Vector2f(val2, val3);
			}

		}else if(arg == "platform"){
			mapf >> val;
			std::string arg2;
			mapf >> arg2;
			if(arg2 == "size"){
				int val2, val3{0};
				mapf >> val2 >> val3;
				platforms[val].Size = Vector2f(val2, val3);
			}
			mapf >> arg2;
			if(arg2 == "pos"){
				int val2, val3{0};
				mapf >> val2 >> val3;
				platforms[val].Position = Vector2f(val2, val3);
				
			}
			

		}else if(arg == "dot"){
			std::string arg2;
			mapf >> arg2;
			if(arg2 == "tre"){
				treasure_txt.loadFromFile("assets/skarb.png");
				dot = 1;
				treasure = new Obstacle[1];
				treasure[0].Size = Vector2f(60,40);
				treasure[0].shape.setTexture(treasure_txt);
				mapf >> arg2;
				delete[] doors;
				if(arg2 == "pos"){
					int val2, val3{0};
					mapf >> val2 >> val3;
					treasure[0].x_y = Vector2f(val2, val3);
				}
				treasure[0].update();
			}else if (arg2 == "door"){
				doors[0].Size = Vector2f(40, 60);
				
				
				mapf >> mapfilename;
				mapf >> arg2;
				if(arg2 == "pos"){
					int val2, val3{0};
					mapf >> val2 >> val3;
					doors[0].x_y = Vector2f(val2, val3);
					doors[0].update();
				}
					
				}

		}else if(arg == "mapsize"){
			mapf >> mapsize.x >> mapsize.y;
			ground.setPosition(Vector2f(0, mapsize.y-33));
			ground.setTextureRect(IntRect(0, 0, mapsize.x, 37));
		}else if(arg == "killertxtnum"){
			int arg2 = 0;
			mapf >> arg2;
			delete[] killer_txt;
			killer_txt = new Texture[arg2];
		}else if(arg == "killertxt"){
			int arg2;
			std::string arg3;
			mapf >> arg2 >> arg3;
			killer_txt[arg2].loadFromFile(arg3);
			killer_txt[arg2].setRepeated(1);

		}else if(arg =="killertxtindex"){
			int ind;
			mapf >> ind;
			std::string index = "";
			bool check = 1;
			while(check){
				mapf >> index;
				if (index != "end"){
					killer[std::stoi(index)].shape.setTexture(killer_txt[ind]);
				}else{check = 0;}
			}

		}
	}
	
	for (int i = 0; i < killernumber; i++)
	{
		killer[i].update();
		
	}

	mapf.close();
}


/*void map2_init() {
	
	killernumber = 1;
	
	player.setPosition(40, 400);
	killer_txt[0].loadFromFile("killer.png");
	killer_txt[0].setRepeated(1);
	
	killer[0].Size = Vector2f(1180, 40);
	killer[0].x_y = Vector2f(100, 680);
	
	platforms[0].Position = Vector2f(160, 620);
	platforms[0].Size = Vector2f(200, 40);
	platforms[1].Position = Vector2f(100, 200);
	platforms[1].Size = Vector2f(400, 40);
	platforms[2].Position = Vector2f(450, 450);
	platforms[2].Size = Vector2f(400, 40);
	platforms[3].Position = Vector2f(700, 150);
	platforms[3].Size = Vector2f(400, 40);
	doors.Size = Vector2f(50, 50);
	doors.x_y = Vector2f(1000, 110);
	doors.shape.setPosition(-1000, -1000);
	for (int i = 0; i < killernumber; i++)
	{
		killer[i].update();
		killer[i].shape.setTexture(killer_txt[0]);
	}
	treasure.x_y = Vector2f(1000, 110);
	treasure.Size = Vector2f(60, 40);
	treasure.update();
	treasure.shape.setTexture(treasure_txt);
} */

	
int main()
{
	//otherplayers.push_back(Sprite(player_txt1, IntRect(0,0,50,50)));
	doors_txt.loadFromFile("assets/door.png");
	std::thread nett(Networking);
	
	doors[0].update();
	doors[0].shape.setTexture(doors_txt);
	player_idle.setSmooth(0);
	player_txt1.setSmooth(0);
	player_txt2.setSmooth(0);
	ground.setTextureRect(IntRect(0, 0, 1280, 37));
	ground.setPosition(Vector2f(0, mapsize.y-33));
	ground.setTexture(middle);
	view.setCenter(1280/2, 720/2);
	view.setSize(Vector2f(1280, 720));
	window.setView(view);
	
	font.loadFromFile("assets/arial.ttf");
	player_idle.loadFromFile("assets/Idle.png");
	std::string nextmapname = "maps/fmap.map";	

	Clock timer;
	player_txt1.loadFromFile("assets/playerrun1.png");
	player_txt2.loadFromFile("assets/playerrun2.png");
	jump_txt.loadFromFile("assets/jump.png");
	player.setPosition(40, 400);
	float fall = 0;
	
	//map_init();
	mapf_init(nextmapname);

	window.setFramerateLimit(120);
	middle.setSmooth(0);
	
	
	while (true) {
		
		if (erroro) return 1;

		Event event;
		window.pollEvent(event);
		if (event.type == Event::Closed)
		{
			stop = 1;
			return 0;
		}
		if (event.type == Event::Resized){
			if (window.getSize().x < 1279){
			view.setSize(window.getSize().x, view.getSize().y);
			}
			if (window.getSize().y < 719){
			view.setSize(view.getSize().x, window.getSize().y);}
			window.setView(view);
			
		}

		if (Keyboard::isKeyPressed(Keyboard::D) || Keyboard::isKeyPressed(Keyboard::Right)) velocity.x += 3;
		if (Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::Left)) velocity.x -= 3;
		if (player.getPosition().y + fall < mapsize.y-80 && !colide()) // jump and colisions
		{
			velocity.y = fall;
			
			fall += 0.08f;
		}
		else
		{
			if (player.getPosition().y > mapsize.y-80)
			{
				player.setPosition(Vector2f(player.getPosition().x, mapsize.y-80));
			}

			if (colide()) {
				
				if (player.getPosition().x + 45 < platforms[colide() - 1].Position.x) {
					if(velocity.x > 0)velocity.x = 0;
					fall = 1;
					velocity.y = fall;
				} else if (player.getPosition().x > (platforms[colide() - 1].Position.x + platforms[colide() - 1].Size.x -5)) {
					if(velocity.x < 0)velocity.x = 0;
					fall = 1;
					velocity.y = fall;

				} else if (platforms[colide() - 1].Position.y > player.getPosition().y) {
					player.setPosition(player.getPosition().x, platforms[colide() - 1].Position.y - 50);
					fall = 0.001f;
					jump_able = true;
				}
				else
				{
					player.setPosition(player.getPosition().x, platforms[colide() - 1].Position.y + 51);
					fall = 0.1;
				}
			}
			else
			{
				fall = 0.001f;
				jump_able = true;
			}

			if (jump_able && (Keyboard::isKeyPressed(Keyboard::Space) || Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W))) fall = -6.5f;
			jump_able = false;
		}

		

		if (player.getPosition().y < 1)
		{
			fall = 0.1f;
			player.setPosition(player.getPosition().x, 5);
		}
		if(!dot){
		if (player.getGlobalBounds().intersects(doors[0].shape.getGlobalBounds()))
		{
			//std::cerr << nextmapname;
			mapf_init(nextmapname);
			
		}
		}

		for (int i = 0; i < killernumber; i++)
		{
			if (killer[i].shape.getGlobalBounds().intersects(player.getGlobalBounds())) {

				player.setPosition(40, 400);
	

			}
		}
		
		//drawing all
		window.clear(Color::Blue);
		window.draw(ground);
		if(!dot)
			doors[0].draw();
		for  (int i = 0; i < platforms_number; i++)
		{
			platforms[i].draw();
		}
		opu.lock();
		for(int i = 0; i < otherplayers.size(); i++){
			window.draw(otherplayers[i]);
		}
		opu.unlock();
		for (int i = 0; i < killernumber; i++)
		{
			killer[i].draw();
		}

		
		
			msm.lock();
			if (timer.getElapsedTime().asSeconds() > 0.2f && !velocity.x == 0)
			{

				if (!playertxt) {
					playertxt = 1;
					player.setTexture(player_txt2);
					
					mside = 0;
					
				}
				else
				{
					playertxt = 0;
					player.setTexture(player_txt1);
					
					mside = 1;
					
				}
				timer.restart();
			}
			else if (velocity.x == 0)
			{
				player.setTexture(player_idle);
				playertxt = 0;
				
				mside = 2;
				
			}
			if (velocity.x < 0)
			{

				if (player.getScale().x > 0)
				{
					player.setPosition(player.getPosition().x + 50, player.getPosition().y);
				}
				player.setScale(-1, 1);


			}
			else {
				if (player.getScale().x < 0)
				{
					player.setPosition(player.getPosition().x - 50, player.getPosition().y);
				}
				player.setScale(1, 1);


			}
		
		
		player.move(velocity);
		possm.lock();
		Positionfornet = player.getPosition();
		possm.unlock();
		
		if (!(fall < 0.1f && fall > -0.1f))
		{
			player.setTexture(jump_txt);
			
			mside = 3;
		}
		if (velocity.x < 0){

			mside +=4;
			
		}
		msm.unlock();
		velocity = Vector2f(0, 0);
		//WORKING ON VIEWS
		if (window.getSize().x < mapsize.x){
			if((!(player.getPosition().x - window.getSize().x/2 <= 50)) && (!(player.getPosition().x + window.getSize().x/2 >= mapsize.x))){
				if(player.getScale().x == 1.0f)
					view.setCenter(player.getPosition().x,view.getCenter().y);
				else
					view.setCenter(player.getPosition().x-50,view.getCenter().y);
			}else if(player.getPosition().x - window.getSize().x/2 <= 50){
				view.setCenter(window.getSize().x/2,view.getCenter().y);	
			}else {
				view.setCenter(mapsize.x - window.getSize().x/2,view.getCenter().y);

			}
		}
		if (window.getSize().y < mapsize.y){
			if((!(player.getPosition().y - window.getSize().y/2 <= 0)) && (!(player.getPosition().y + window.getSize().y/2 >= mapsize.y))){
			view.setCenter(view.getCenter().x, player.getPosition().y);
			}else if(player.getPosition().y - window.getSize().y/2 <= 0){
				view.setCenter(view.getCenter().x,window.getSize().y/2);	
			}else {
				view.setCenter(view.getCenter().x, mapsize.y - window.getSize().y/2);

			}
		}
		window.setView(view);
		

		next_position = player.getGlobalBounds();
		if(dot)
		treasure[0].draw();
		window.draw(player);

		
			
		
		
		
		window.display();
		if (dot){
		if (player.getGlobalBounds().intersects(treasure[0].shape.getGlobalBounds()))
		{
			break;
		}
		}


	}
	
	stop = 1;
	
	You_Won.setString("You Won");
	You_Won.setFont(font);
	You_Won.setCharacterSize(300);
	You_Won.setPosition(50, 150);
	Clock clock;
	view.setCenter(1280/2, 720/2);
	view.setSize(1280, 720);
	window.setView(view);
	while (clock.getElapsedTime().asSeconds() < 5)
	{

		window.clear(Color::Blue);
		window.draw(You_Won);
		window.display();
	}



}	

