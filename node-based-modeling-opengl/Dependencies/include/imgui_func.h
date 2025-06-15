#ifndef  IMGUI_FUNC_H
#define IMGUI_FUNC_H


#include "imgui/imgui.h"

void button_list_math(
	int& select_num,
	const char* names[],
	const int names_count
)
{

	if (ImGui::Button("Math"))
		ImGui::OpenPopup("my_select_popup_math");
	ImGui::SameLine();
	//ImGui::TextUnformatted(select_num == -1 ? "<None>" : names[select_num]);
	if (ImGui::BeginPopup("my_select_popup_math"))
	{
		ImGui::SeparatorText("const char");
		for (int i = 0; i < names_count ; i++)
		{
			if (ImGui::Selectable(names[i]))
			{
				select_num = i;
			}
		}

			

		ImGui::EndPopup();
		
	}

}
void button_list_gen_object(
	int& select_num,
	const char* names[],
	const int names_count
)
{

	if (ImGui::Button("Gen_object"))
		ImGui::OpenPopup("my_select_popup_gen_object");
	ImGui::SameLine();
	//ImGui::TextUnformatted(select_num == -1 ? "<None>" : names[select_num]);
	if (ImGui::BeginPopup("my_select_popup_gen_object"))
	{
		ImGui::SeparatorText("const char");
		for (int i = 0; i < names_count; i++)
		{
			if (ImGui::Selectable(names[i]))
			{
				select_num = i;
			}
		}



		ImGui::EndPopup();

	}

}

void button_list_animation(
	int& select_num,
	const char* names[],
	const int names_count
)
{

	if (ImGui::Button("animation"))
		ImGui::OpenPopup("my_select_popup_animation");
	ImGui::SameLine();
	//ImGui::TextUnformatted(select_num == -1 ? "<None>" : names[select_num]);
	if (ImGui::BeginPopup("my_select_popup_animation"))
	{
		ImGui::SeparatorText("const char");
		for (int i = 0; i < names_count; i++)
		{
			if (ImGui::Selectable(names[i]))
			{
				select_num = i;
			}
		}



		ImGui::EndPopup();

	}

}



template <typename MapType>
void button_list(
	std::string& select_name,
	const MapType& names_map
)
{
	

	if (ImGui::Button("Object model"))
		ImGui::OpenPopup("my_select_popup");
	ImGui::SameLine();

	std::vector<std::string> names_list;

	for (const auto& [name, val] : names_map)
	{
		names_list.push_back(name);


	}
	int names_count = names_list.size();


	if (ImGui::BeginPopup("my_select_popup"))
	{
		ImGui::SeparatorText("map");
		for (int i = 0; i < names_count; i++)
		{
			if (ImGui::Selectable((names_list[i]).c_str()))
			{

				select_name = names_list[i];
			}
		}



		ImGui::EndPopup();

	}



}





#endif
