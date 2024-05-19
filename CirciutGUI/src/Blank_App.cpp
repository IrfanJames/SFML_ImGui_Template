
/*
	* Colapse all Scopes - it'll be easier
	* Ctrl + M + A in Visual Studio
*/

#include <iostream>
#include <vector>
#include <fstream>
#include <direct.h>
#include <chrono>
#include <future>

//#include <thread>
//#include <sstream>
//#include <string>

#include "imgui.h"
#include "imgui-SFML.h"
#include "SFML/Graphics.hpp"

#include "LOG.hpp"
#include "Timer.hpp"
#include "Windows_Stuff.hpp"

#include "Blank_GUI.hpp"
#include "Blank_App.hpp"


App::App(const std::vector<std::string>& filepaths)
{
	srand(time(NULL));

	GUI::initializeGUI();

	ImGui::SFML::Init(GUI::app);
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;


	//Resource my_pic(IDR_CAP, "BMP");
	//Resource my_font(IDR_FONT2, "FONT");
	//my_font.GetInfo();

	/* Test Circle
	bool DrawCircle = 1; int t_vertices = 34; float t_radius = 50, t_Colors[3] = { (float)204 / 255, (float)77 / 255, (float)5 / 255 }; sf::CircleShape testCircle(t_radius, t_vertices); {
		testCircle.setOrigin(t_radius, t_radius);
		testCircle.setPosition(W / 2, H / 2);
		testCircle.setFillColor(sf::Color((int)(t_Colors[0] * 255), (int)(t_Colors[1] * 255), (int)(t_Colors[2] * 255))); }//*/

#ifdef _DEBUG
	// For Opening Where Left
	GUI::Options::Open("temp_files/last.txt");
#endif

	//////////////// argv //////////////////
	if (filepaths.empty() == false)
	{
		GUI::Options::Open(filepaths.back());
	}
}

App::~App()
{
	//ImGui
	ImGui::SFML::Shutdown();

#ifdef _DEBUG
	GUI::Options::Save("temp_files/last.txt");
#else
	LOG::log_file.close();
#endif
}

void App::Run()
{
	//////////////// Main Loop ////////////////////
	while (GUI::app.isOpen() && End == false)
	{
		Events();

		ImGUI();

		Options();

		Threads();

		Update();

		Render();

		EndFrame();
	}

}



void App::Events()
{
	while (GUI::app.pollEvent(GUI::evnt)) {
		using namespace GUI;

		// ImGui
		ImGui::SFML::ProcessEvent(evnt);

		if (evnt.type == evnt.Closed) {

			std::ofstream window_size("temp_files/win_size.txt");
			window_size << GUI::app.getSize().x << "\t";
			window_size << GUI::app.getSize().y << "\t";
			window_size << GUI::app.getPosition().x << "\t";
			window_size << GUI::app.getPosition().y << "\t";
			window_size.close();

			app.close(); End = 1;
		}

		if (evnt.type == evnt.MouseLeft) { cursorInWin = 0; }

		if (evnt.type == evnt.MouseEntered) { cursorInWin = 1; }

		if (evnt.type == evnt.Resized) {
			view = sf::View(sf::FloatRect(
				(int)view.getCenter().x - (int)(evnt.size.width / 2),
				(int)view.getCenter().y - (int)(evnt.size.height / 2),
				(int)evnt.size.width,
				(int)evnt.size.height));
			//CircuitGUI::view.setSize((int)evnt.size.width, (int)evnt.size.height);
			//CircuitGUI::view.setCenter((int)CircuitGUI::view.getCenter().x, (int)CircuitGUI::view.getCenter().y);


			std::ofstream window_size("temp_files/win_size.txt");
			window_size << GUI::app.getSize().x << "\t";
			window_size << GUI::app.getSize().y << "\t";
			window_size << GUI::app.getPosition().x << "\t";
			window_size << GUI::app.getPosition().y << "\t";
			window_size.close();
		}

		if (evnt.type == evnt.MouseButtonPressed)
		{
			if (evnt.mouseButton.button == sf::Mouse::Middle) {
				dragBool = true;
				GUI::iniDrag();
			}
		}

		if (evnt.type == evnt.MouseButtonReleased)
		{
			if (evnt.mouseButton.button == sf::Mouse::Middle) {
				dragBool = false;
			}
		}

		if (evnt.type == evnt.KeyPressed)
		{
			if (evnt.key.code == sf::Keyboard::Escape) {
				app.close();
				End = 1;
				continue;
			}

			// Ctrl
			if (evnt.key.control) {
				if (evnt.key.code == sf::Keyboard::O) {

					std::string filepath = OpenFileDialog("text file (*.txt)\0*.txt\0");

					if (!filepath.empty())
						GUI::Options::Open(filepath);

				}
				if (evnt.key.code == sf::Keyboard::S) {
					;
					//_mkdir("Saved-Images");		// Alredy Exists(-1) else Created(0)
					//_mkdir("Saved-Projects");	// Alredy Exists(-1) else Created(0)
				}
				if (evnt.key.code == sf::Keyboard::S && evnt.key.shift) {

					_mkdir("Saved-Images");		// Alredy Exists(-1) else Created(0)
					_mkdir("Saved-Projects");	// Alredy Exists(-1) else Created(0)
					std::string filepath = SaveFileDialog("Project file (*.TXT)\0*.TXT\0PNG (*.PNG)\0*.PNG\0");//JPEG (*.JPG)\0*.JPG\0

					if (!filepath.empty()) {

						if (filepath.back() == 'T')
							GUI::Options::Save(filepath);

						if (filepath.back() == 'G')
							futures.emplace_back(std::async(std::launch::async, GUI::Options::SaveAsImage, filepath));

					}

				}
			}
		}

	}


}

void App::ImGUI() {
	using namespace GUI;
	ImGuiInFocus = false;

	ImGui::SFML::Update(app, deltaClock.restart());
	/*
	ImGui::Begin("Frist ImGui Win");
	ImGui::Text("My Project will be on Steroids");
	ImGui::Checkbox("Draw Circle", &DrawCircle);
	ImGui::SliderFloat("Radius", &t_radius, 0, 200);
	ImGui::SliderInt("Sides", &t_vertices, 3, 35);
	ImGui::ColorEdit3("Color", t_Colors);
	ImGui::End();

	testCircle.setRadius(t_radius);
	testCircle.setOrigin(testCircle.getRadius(), testCircle.getRadius());
	testCircle.setPointCount(t_vertices);
	testCircle.setFillColor(sf::Color((int)(t_Colors[0] * 255), (int)(t_Colors[1] * 255), (int)(t_Colors[2] * 255)));//*/

	if (ImGui::BeginMainMenuBar())
	{
		if (/*ImGui::IsWindowFocused() || */ ImGui::IsWindowHovered()) {
			//LOG("\nMainBar");
			ImGuiInFocus = true;
		}
		if (ImGui::BeginMenu("File"))
		{
			ImGuiInFocus = true;

			if (ImGui::MenuItem("New", "Ctrl + N")) {
				// stimuliDisplay = 1;
			}
			if (ImGui::MenuItem("Open...", "Ctrl + O")) {

				std::string filepath = OpenFileDialog("text file (*.txt)\0*.txt\0");

				if (!filepath.empty())
					GUI::Options::Open(filepath);
			}
			if (ImGui::MenuItem("Save", "Ctrl + S")) {
				;
				//_mkdir("Saved-Images"); // Alredy Exists(-1) else Created(0)
				//_mkdir("Saved-Projects"); // Alredy Exists(-1) else Created(0)
			}
			if (ImGui::MenuItem("Save As...", "Ctrl + Shift + S")) {

				_mkdir("Saved-Images"); // Alredy Exists(-1) else Created(0)
				_mkdir("Saved-Projects"); // Alredy Exists(-1) else Created(0)

				std::string filepath = SaveFileDialog("Project file (*.TXT)\0*.TXT\0PNG (*.PNG)\0*.PNG\0");//JPEG (*.JPG)\0*.JPG\0

				if (!filepath.empty()) {

					if (filepath.back() == 'T')
						GUI::Options::Save(filepath);

					if (filepath.back() == 'G')
						GUI::Options::SaveAsImage(filepath);
						//futures.emplace_back(std::async(std::launch::async, CircuitGUI::Options::SaveAsImage, filepath));
				}

			}
			ImGui::Separator();
			if (ImGui::MenuItem("Exit", "Esc")) { GUI::app.close(); End = 1; /*continue;*/ }

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Options"))
		{
			ImGuiInFocus = true;

			if (ImGui::BeginMenu("Handdrawn Icons"))
			{
				ImGui::Text("Coming Soon...");
				ImGui::EndMenu();
			}
			ImGui::Separator();
			//ImGui::MenuItem("Show QuadTree", "", &visible_QuadTree);
			if (ImGui::BeginMenu("Theme")) {

				if (ImGui::MenuItem("Dark")) {

					darkLightMode = false;

					updateThemeColors();
				}
				if (ImGui::MenuItem("Light")) {

					darkLightMode = true;

					updateThemeColors();
				}

				ImGui::EndMenu();
			}


			//asdf
			/*ImGui::Separator();
			if (ImGui::MenuItem("Game")) {
				stimuliDisplay = 1; stimuliEndNodes = 1; //cout << "17";
				PlayMode = !PlayMode;

				CircuitGUI::comp.clear();
				CircuitGUI::virSerial.clear();
				CircuitGUI::virSprite.clear();
				CircuitGUI::virSerialShift.clear();

				//asdfwires.clear();

				if (PlayMode) {
					CircuitGUI::Options::openf("Saved-Projects\\Maze.TXT");
					//ShellExecute(0, 0, L"https://www.youtube.com/watch?v=6cRctjPRv6M", 0, 0, SW_SHOW);
				}
				else { wireBool = 0; PlayRot = 0; }
			}*/

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
			ImGuiInFocus = true;

			if (ImGui::BeginMenu("Controls"))
			{
				if (ImGui::BeginTable("table_context_menu_2", 2))
				{
					static const std::string opt[11][2] = { //HardCode
						{ "Drag View", "Press Scroll-Wheel + Drag" },
						{ "Selection Mode", "Click + Hold + Drag" },
						{ "Select All",		"Ctrl + A" },
						{ "Rotate",			"Ctrl + R" },
						{ "Delete",			"Del" },
						{ "Copy",			"Ctrl + C" },
						{ "Paste",			"Ctrl + V" },
						{ "Open",			"Ctrl + O" },
						{ "Save As",		"Ctrl + Shift + S" },
						{ "Escape",			"Esc" },
						{ "New Components",	"Hover Left" }
					};

					ImGui::TableSetupColumn("Option");
					ImGui::TableSetupColumn("Keys");

					ImGui::TableHeadersRow();
					ImGui::TableNextRow();

					for (int row = 0; row < 11; row++) //HardCode
					{
						ImGui::TableSetColumnIndex(0);
						ImGui::Text("%s", opt[row][0].c_str());
						ImGui::SameLine();

						ImGui::TableSetColumnIndex(1);
						//ImGui::Text(opt[row][1].c_str());

						bool sameLine = false;
						std::string Text = opt[row][1] + " ", temp = "";
						for (int i = 0; i < Text.length(); i++) {

							if (('0' <= Text[i] && Text[i] <= '9') || ('a' <= Text[i] && Text[i] <= 'z') || ('A' <= Text[i] && Text[i] <= 'Z')) {
								temp += Text[i];
							}
							else if (Text[i] == '+') {
								if (sameLine) ImGui::SameLine();
								ImGui::Text("+");
								sameLine = true;
							}
							else {
								if (temp.size()) {
									if (sameLine) ImGui::SameLine();
									ImGui::SmallButton(temp.c_str());
									temp.clear();
									sameLine = true;
								}
							}

						}

						ImGui::TableNextRow();
					}
					ImGui::EndTable();
				}

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Contacts"))
			{
				static std::string links[2][2] = // HardCode
				{
					{ "Copy Github", "https://github.com/IrfanJames/CircuitSimulator_SFML" },
					{ "Copy Email ", "irfanjamespak@gmail.com" }
				};

				for (int i = 0; i < 2; i++) {
					if (ImGui::Button(links[i][0].c_str()))
						sf::Clipboard::setString(links[i][1]);
					ImGui::SameLine();
					ImGui::Text("%s", links[i][1].c_str());
				}

				ImGui::EndMenu();
			}

			/*ImGui::Separator();

			if (ImGui::BeginMenu("Options"))
			{
				static bool enabled = true;
				ImGui::MenuItem("Enabled", "", &enabled);
				ImGui::BeginChild("child", ImVec2(0, 60), true);
				for (int i = 0; i < 10; i++)
					ImGui::Text("Scrolling Text %d", i);
				ImGui::EndChild();
				static float f = 0.5f;
				static int n = 0;
				ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
				ImGui::InputFloat("Input", &f, 0.1f);
				ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
				ImGui::EndMenu();
			}*/

			ImGui::EndMenu();
		}

		/*ImGui::Begin("Right-Click");
		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::Selectable("Apple")) LOG("\nApple");
			if (ImGui::Selectable("Banana")) LOG("\nBanana");
			ImGui::EndPopup();
		}
		ImGui::End;

		if (ImGui::BeginPopup("Set Value")) {

			static float value = 0;
			ImGui::DragFloat("Value", &value);

			comp[virSerial.front()].voltage = value;

		}
		ImGui::EndPopup();*/


		//if (ImGui::SmallButton("AD")) {
		//	LOG("\nAds");
		//}
		//
		//if (ImGui::Button("ad")) {
		//	LOG("\nAds");
		//}

		ImGui::EndMainMenuBar();
	}

	//Docking
	; {
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
	}

	if (ImGui::Begin("Window-A##ID"))
	{
		if (ImGui::IsWindowFocused() || ImGui::IsWindowHovered())
			ImGuiInFocus = true;

		// HiddenLayers
		{
			ImGui::DragInt("Value", &value, 0.1);
			if (value < 0)
				value = 0;
			//ImGui::SliderInt("Hidden Layers", &hiddenLayerCount, 0, 12);
		}
	}
	ImGui::End();
}

void App::Options() {

	if (ImGuiInFocus) return;


}

void App::Threads()
{
	for (int i = 0; i < futures.size(); i++) {

		std::future_status status = futures[i].wait_for(std::chrono::milliseconds(10));

		if (status == std::future_status::ready) //{
		// The future is ready (thread completed or value available)
		//	LOG("ready");
			futures.erase(futures.begin() + i--);
		//}
		//else if (status == std::future_status::timeout) {
		//	// The future is not ready within the specified duration
		//	LOG("timeout");
		//}
		//else if (status == std::future_status::deferred) {
		//	// The future is deferred (using std::promise and std::async(launch::deferred))
		//	LOG("deferred");
		//}

	}
}

void App::Update()
{
	using namespace GUI;

	// Drag
	{
		if (dragBool) {
			GUI::Drag();
			GUI::colorBrightLineGrid();
		}
	}
}

void App::Render()
{
	using namespace GUI;

	app.setView(view);

	app.clear(backColor);

	drawGrid();

	ImGui::SFML::Render(app); // Last Thing to render

	// CircuitGUI::app.display(); // Inside void App::EndFrame()
}

void App::EndFrame()
{
	GUI::app.display();

	// FPS
	{
		static Timer FPS_Timer;
		double duration = FPS_Timer.duration();
		GUI::app.setTitle("Blank App   " + std::to_string(duration) + " | " + std::to_string(1000 / duration));
		FPS_Timer.reset();
	}
}

