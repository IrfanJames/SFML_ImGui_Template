#pragma once

#include <string>
#include <vector>
#include <future>

#include "SFML/System/Clock.hpp"


class App {

public:

	App(const std::vector<std::string>& filepaths);

	~App();

	void Run();


private:

	void Events();

	void ImGUI();

	void Options();

	void Threads();

	void Update();

	void Render();

	void EndFrame();


private:

	bool End = 0;
	bool cursorInWin = 0;
	bool ImGuiInFocus = 0;

	int value = 0;


	// ImGui
	sf::Clock deltaClock;

	std::vector<std::future<void>> futures;
};