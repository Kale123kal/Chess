#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Game.h"

namespace Chess
{
	static std::weak_ptr<Game> s_GameLayerInstance;

	std::shared_ptr<Game> Game::Get()
	{
		return s_GameLayerInstance.lock();
	}
}
Walnut::Application *Walnut::CreateApplication(int argc, char **argv)
{
	using namespace Chess;

	Walnut::ApplicationSpecification spec;
	spec.Name = "Chess";

	Walnut::Application *app = new Walnut::Application(spec);
	std::shared_ptr<Game> gameLayer = std::make_shared<Game>();

	app->PushLayer(gameLayer);
	s_GameLayerInstance = gameLayer;
	app->SetMenubarCallback([app]()
							{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		} });
	return app;
}
