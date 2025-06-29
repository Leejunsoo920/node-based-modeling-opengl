#define IMGUI_DEFINE_MATH_OPERATORS

#include "Node.h"
#include "imgui_func.h"
#include "ImGuizmo.h"
#include <glm/gtc/type_ptr.hpp>
#include <chrono>

#include <ctime>
#include <sstream>
#include <regex>

extern Frame_manager frame_manager;

//hash func to using string in swith
// 0xEDB88320 is CRC32 algorithm
constexpr unsigned int hash_table(const char* str)
{
	////check const char* by ternary operator
	return str[0] ? static_cast<unsigned int>(str[0]) + 0xEDB8832Full * hash_table(str + 1) : 8603;//// if str[0] == null -> 8603
}


void Node::imgui_render()
{
	ImGui::BeginChild((name).c_str(), ImVec2(250, 200), ImGuiWindowFlags_NoTitleBar);
	{
		ImGui::Text((name).c_str());
	}
	ImGui::EndChild();
}

bool GraphEditorDelegate::AllowedLink(
	GraphEditor::NodeIndex from,
	GraphEditor::NodeIndex to
)
{
	return true;
}

void GraphEditorDelegate::SelectNode(
	GraphEditor::NodeIndex nodeIndex,
	bool selected
)
{
	mNodes[nodeIndex]->mSelected = selected;
}
void GraphEditorDelegate::MoveSelectedNodes(
	const ImVec2 delta
)
{
	for (auto& node : mNodes)
	{
		if (!node->mSelected)
		{
			continue;
		}
		node->x += delta.x;
		node->y += delta.y;
	}
}
void GraphEditorDelegate::RightClick(GraphEditor::NodeIndex nodeIndex, GraphEditor::SlotIndex slotIndexInput, GraphEditor::SlotIndex slotIndexOutput)
{

}
void GraphEditorDelegate::AddLink(
	GraphEditor::NodeIndex inputNodeIndex,
	GraphEditor::SlotIndex inputSlotIndex,
	GraphEditor::NodeIndex outputNodeIndex,
	GraphEditor::SlotIndex outputSlotIndex
)
{

	auto copy_before_mLinks = mLinks;

	mLinks.push_back({ inputNodeIndex, inputSlotIndex, outputNodeIndex, outputSlotIndex });


	if (topological_sort(mNodes, mLinks))
	{
		node_changed = true;
	}
	else
	{
		mLinks = copy_before_mLinks;

	}
}
void GraphEditorDelegate::DelLink(
	GraphEditor::LinkIndex linkIndex
)
{
	auto copy_before_mLinks = mLinks;
	auto this_node_index = mLinks[linkIndex].mOutputNodeIndex;
	mLinks.erase(mLinks.begin() + linkIndex);
	if (topological_sort(mNodes, mLinks))
	{
		node_changed = true;
		mNodes[this_node_index]->evaluated = false;
	}
	else
	{
		mLinks = copy_before_mLinks;
	}

}
void GraphEditorDelegate::CustomDraw(
	ImDrawList* drawList,
	ImRect rectangle,
	GraphEditor::NodeIndex nodeIndex)
{
	drawList->AddLine(rectangle.Min, rectangle.Max, IM_COL32(0, 0, 0, 255));
	drawList->AddText(rectangle.Min, IM_COL32(255, 128, 64, 255), "Draw");
}
const size_t  GraphEditorDelegate::GetTemplateCount()
{
	return sizeof(GraphEditorDelegate::mTemplates) / sizeof(GraphEditor::Template);
}
const GraphEditor::Template GraphEditorDelegate::GetTemplate(
	GraphEditor::TemplateIndex index
)
{
	return mTemplates[index];
}
const size_t GraphEditorDelegate::GetNodeCount(

)
{
	return mNodes.size();
}
const GraphEditor::Node GraphEditorDelegate::GetNode(
	GraphEditor::NodeIndex index
)
{
	int d = mNodes[index]->templateIndex;
	return GraphEditor::Node
	{
		(mNodes[index]->name).c_str(),
		mNodes[index]->templateIndex,
		ImRect(ImVec2(mNodes[index]->x, mNodes[index]->y), ImVec2(mNodes[index]->x + 200, mNodes[index]->y + 200)),
		mNodes[index]->mSelected
	};
}
const size_t GraphEditorDelegate::GetLinkCount()
{
	return this->mLinks.size();
}


const GraphEditor::Link GraphEditorDelegate::GetLink(
	GraphEditor::LinkIndex index
)
{
	return this->mLinks[index];
}


template <typename T> 
void GraphEditorDelegate::gen_node(ImVec2 nodeRectangleMin, const char* node_name, const T& ani)
{
	auto copy_before_mNodes = mNodes;

	ImGuiIO& im_io = ImGui::GetIO();
	static ImVec2 quadSelectPos;
	static int node_count = 0;
	std::string new_name;
	switch (hash_table(node_name))
	{
	case hash_table("float"):
	{
		new_name = "Float";
		std::shared_ptr<Node> new_node = std::make_shared<Node_value_float>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	case hash_table("Int"):
	{
		new_name = "Int";
		std::shared_ptr<Node> new_node = std::make_shared<Node_value_int>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	case hash_table("mat4"):
	{
		new_name = "mat4";
		std::shared_ptr<Node> new_node = std::make_shared<Node_value_mat4>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	case hash_table("add"):
	{
		new_name = "add";
		std::shared_ptr<Node> new_node = std::make_shared<Node_func_add>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	case hash_table("result"):
	{
		new_name = "result";
		std::shared_ptr<Node> new_node = std::make_shared<Node_func_result>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	case hash_table("subtract"):
	{
		new_name = "subtract";
		std::shared_ptr<Node> new_node = std::make_shared<Node_func_subtract>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	case hash_table("divide"):
	{
		new_name = "divide";
		std::shared_ptr<Node> new_node = std::make_shared<Node_func_divide>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	case hash_table("multiply"):
	{
		new_name = "multiply";
		std::shared_ptr<Node> new_node = std::make_shared<Node_func_multiply>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	case hash_table("vec3"):
	{
		new_name = "vec3";
		std::shared_ptr<Node> new_node = std::make_shared<Node_value_vec3>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	case hash_table("animation"):
	{
		new_name = "animation";
		std::shared_ptr<Node> new_node = std::make_shared<Node_animation>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false, ani);
		mNodes.push_back(std::move(new_node));
		break;
	}
	case hash_table("line"):
	{
		new_name = "line";
		std::shared_ptr<Node> new_node = std::make_shared<Node_func_line>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}

	case hash_table("line_to_global_matrix"):
	{
		new_name = "line_to_global_matrix";
		std::shared_ptr<Node> new_node = std::make_shared<Node_func_line_to_global_matrix>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	case hash_table("bezier_curve"):
	{
		new_name = "bezier curve";
		std::shared_ptr<Node> new_node = std::make_shared<Node_func_bezier_curve>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	case hash_table("cubic_bezier_curve"):
	{
		new_name = "cubic_bezier_curve";
		std::shared_ptr<Node> new_node = std::make_shared<Node_func_cubic_bezier_curve>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	
	case hash_table("sweep"):
	{
		new_name = "sweep";
		std::shared_ptr<Node> new_node = std::make_shared<Node_func_sweep>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	case hash_table("select_one_value"):
	{
		new_name = "select_one_value";
		std::shared_ptr<Node> new_node = std::make_shared<Node_func_select_one_value>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	case hash_table("mat4_rotate"):
	{
		new_name = "mat4_rotate";
		std::shared_ptr<Node> new_node = std::make_shared<Node_func_matrix_rotation>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	case hash_table("mat4_z_lookup"):
	{
		new_name = "mat4_z_lookup";
		std::shared_ptr<Node> new_node = std::make_shared<Node_func_matrix_z_look_up>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	case hash_table("now_frame"):
	{
		new_name = "now_frame";
		std::shared_ptr<Node> new_node = std::make_shared<Node_func_now_frame>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false);
		mNodes.push_back(std::move(new_node));
		break;
	}
	}
	node_count++;
	if (topological_sort(mNodes, mLinks))
	{
		node_changed = true;
	}
	else
	{
		mNodes = copy_before_mNodes;
	}

}

template <typename T, typename U>
void GraphEditorDelegate::gen_node(ImVec2 nodeRectangleMin, const char* node_name, const T& object,int& object_count, const U& shader)
{
	auto copy_before_mNodes = mNodes;
	ImGuiIO& im_io = ImGui::GetIO();
	static ImVec2 quadSelectPos;
	debug.set_text("mouse pos", glm::vec2(nodeRectangleMin.x, nodeRectangleMin.y));
	static int node_count = 0;
	std::string new_name;
	switch (hash_table(node_name))
	{

	case hash_table("object"):
	{
		if (object != nullptr)
		{

			new_name = "object" + object->name;
			std::shared_ptr<Node> new_node = std::make_shared<Node_object>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false, object, object_count, shader);
			mNodes.push_back(std::move(new_node));

			object_count++;

		}

		break;
	}
	
	case hash_table("circle"):
	{
		std::string object_name = "circle";

		new_name = "gen_object" + object_name;
		std::shared_ptr<Node> new_node = std::make_shared<Node_generate_circle_object>(new_name, node_count, 0, im_io.MousePos.x - 610, im_io.MousePos.y - 550, false, nullptr, object_count, shader);
		mNodes.push_back(std::move(new_node));
		object_count++;
		break;
	}
	
	}
	node_count++;
	if (topological_sort(mNodes, mLinks))
	{
		node_changed = true;
	}
	else
	{
		mNodes = copy_before_mNodes;
	}
}

void GraphEditorDelegate::del_node()
{
	auto copy_before_mNodes = mNodes;

	for (auto it = mNodes.begin(); it != mNodes.end(); ) 
	{
		if ((*it)->mSelected)
		{

			int node_index = it - mNodes.begin();

			for (auto link_it = mLinks.begin(); link_it != mLinks.end(); )
			{
				if (link_it->mInputNodeIndex == node_index || link_it->mOutputNodeIndex == node_index)
				{
					link_it = mLinks.erase(link_it);
				}
				else
				{
					++link_it;
				}
			}
			for (auto& link : mLinks)
			{
				if (link.mInputNodeIndex > node_index)
				{
					link.mInputNodeIndex -= 1;
				}
				if (link.mOutputNodeIndex > node_index)
				{
					link.mOutputNodeIndex -= 1;
				}
			}
			it = mNodes.erase(it);  
		}
		else {
			++it; 
		}
	}

	if (topological_sort(mNodes, mLinks))
	{
		node_changed = true;
	}
	else
	{
		mNodes = copy_before_mNodes;
	}
}

bool GraphEditorDelegate::topological_sort(
	const std::vector<std::shared_ptr<Node>>& mNodes,
	const std::vector<GraphEditor::Link>& mlinks
)
{
	auto copy_before_sorted_graph = this->sorted_graph;

	int node_size = mNodes.size();

	std::vector<std::vector<int>> graph(node_size);
	std::vector<int> inDegree(node_size, 0);

	//// 1. generate graph by link
	for (const auto& link : mlinks)
	{
		int from = link.mOutputNodeIndex;
		int to = link.mInputNodeIndex;

		graph[from].push_back(to);
		inDegree[to]++;
	}

	//// 2. input node that indegree == 0 in queue
	std::queue<int> q;
	for (int i = 0; i < node_size; i++)
	{
		if (inDegree[i] == 0)
			q.push(i);
	}


	//// 3. topological sort
	std::vector<int> sortedOrder;
	while (!q.empty())
	{
		int curr = q.front();
		q.pop();
		sortedOrder.push_back(curr);

		for (int next : graph[curr])
		{
			inDegree[next]--;
			if (inDegree[next] == 0)
			{
				q.push(next);
			}

		}
	}

	//// 4. check cycle
	if (sortedOrder.size() != node_size)
	{
		sortedOrder.clear();
		this->sorted_graph = copy_before_sorted_graph;
		return false;

	}
	else
	{

		std::reverse(sortedOrder.begin(), sortedOrder.end());
		this->sorted_graph = sortedOrder;

		for (const auto& node : mNodes)
		{
			node->input_node.clear();
			node->output_node_num = 0;
		}
		//// set inputnode pointer to after node
		for (const auto& link : mlinks)
		{
			int from = link.mOutputNodeIndex;
			int to = link.mInputNodeIndex;

			mNodes[from]->input_node.push_back(mNodes[to]);
			mNodes[to]->output_node_num++;
		}
		return true;
	}
}


/////////////////////////////////////////////////////////////////////////////////////////

void Node_manager::update(
	const glm::mat4& projection,
	const glm::mat4& view,
	const glm::vec3& camera_position)
{
	//// set frame system /// frame manager is the ctrl to system frame
	frame_manager.update();

	auto iter = pre_shader_map.begin();
	auto iter_ob = pre_object_map.begin();

	//// gen, delete node
	if (gen_node_click)
	{

		select_gen_node(pre_object_map);
	}

	if (ImGui::IsKeyPressed(ImGuiKey_Delete))
	{
		delegate.del_node();
	}


	if (inputctrl_global->mesh_fill)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	
	debug_UI_draw(projection,view);
	imgui_render();



	imgui_node_property_render(projection, view, camera_position);



	if (first_run)
	{

		first_run = false;
	}




	
}

void Node_manager::select_gen_node(objects_map_t& pre_object_map)
{
	ImGui::Begin("node select");
	{
		//ImGui::SetWindowFocus();

		static int select_node_num = -1;
		static int reseach_node_num = -1;
		static int select_gen_object_num = -1;

		const char* math_node_names[] = 
		{
			"float",
			"Int",
			"mat4",
			"add",
			"result",
			"subtract",
			"divide",
			"multiply",
			"vec3",
			"object",
			"animation",
			"vector_vec3",
			"line",
			"line_to_global_matrix",
			"bezier_curve",
			"cubic_bezier_curve",
			"sweep",
			"set_keyframe",
			"select_one_value",
			"mat4_rotate",
			"mat4_z_lookup",
			"trilinear_interpolation",
			"mesh_projection",
			"now_frame"
		};

		const char* animation_node_names[] =
		{
			"animation"
		};

		const char* gen_object_names[] =
		{
			"circle"
		};
		int name_count = IM_ARRAYSIZE(math_node_names);
		int gen_object_name_count = IM_ARRAYSIZE(gen_object_names);

		button_list_math(select_node_num, math_node_names, name_count);
		button_list_animation(reseach_node_num, animation_node_names, 1);
		button_list_gen_object(select_gen_object_num, gen_object_names, gen_object_name_count);
		std::string select_object = "none";
		button_list(select_object, pre_object_map);
		if (select_object != "none")
		{
			ImVec2 temp = ImVec2(0, 0);
			ImVec2 offset = ImGui::GetCursorScreenPos() + viewState.mPosition * viewState.mFactor;
			ImVec2 nodeRectangleMin = offset + temp * viewState.mFactor;
			delegate.gen_node(nodeRectangleMin, "object", pre_object_map[select_object],this->object_count, pre_shader_map["skin"]);
		}
		if (select_gen_object_num != -1)
		{
			ImVec2 temp = ImVec2(0, 0);
			ImVec2 offset = ImGui::GetCursorScreenPos() + viewState.mPosition * viewState.mFactor;
			ImVec2 nodeRectangleMin = offset + temp * viewState.mFactor;
			delegate.gen_node(nodeRectangleMin, gen_object_names[select_gen_object_num], pre_object_map["circle"], this->object_count, pre_shader_map["nomal shader"]);
			select_gen_object_num = -1;
		}


		if(select_node_num != -1)
		{
			ImVec2 temp = ImVec2(0, 0);
			ImVec2 offset = ImGui::GetCursorScreenPos() + viewState.mPosition * viewState.mFactor;
			ImVec2 nodeRectangleMin = offset + temp * viewState.mFactor;
			delegate.gen_node(nodeRectangleMin, math_node_names[select_node_num], nullptr);
			select_node_num = -1;
		}

		if (reseach_node_num != -1)
		{
			ImVec2 temp = ImVec2(0, 0);
			ImVec2 offset = ImGui::GetCursorScreenPos() + viewState.mPosition * viewState.mFactor;
			ImVec2 nodeRectangleMin = offset + temp * viewState.mFactor;
			delegate.gen_node(nodeRectangleMin, animation_node_names[reseach_node_num], pre_animation_map["run"]);
			reseach_node_num = -1;
		}


	}
	ImGui::End();
}

void Node_manager::imgui_render()
{

	ImGui::Begin("node editor controls");
	if (ImGui::CollapsingHeader("Graph Editor")) 
	{
		ImGui::Checkbox("Show GraphEditor", &showGraphEditor);
		GraphEditor::EditOptions(options);
	}

	ImGui::End();

	debug.set_text("n", glm::vec3(0, 0, 0));


	if (showGraphEditor)
	{
		ImGui::Begin("Graph Editor", NULL, 0);
		if (ImGui::Button("Fit all nodes"))
		{
			fit = GraphEditor::Fit_AllNodes;
		}
		ImGui::SameLine();
		if (ImGui::Button("Fit selected nodes"))
		{
			fit = GraphEditor::Fit_SelectedNodes;
		}
		ImGui::SameLine();
		
		////node save system 
		nlohmann::json j;
		ImGui::SameLine();
		if (ImGui::Button("save all node"))
		{
			//// get node imgui visualize option
			j["view_position"] = { viewState.mPosition.x, viewState.mPosition.y };
			j["view_factor"] = viewState.mFactor;
			ImVec2 offset = ImGui::GetCursorScreenPos() + viewState.mPosition * viewState.mFactor;
			j["view_offset"] = { offset.x, offset.y };
			

			//// get nodes option
			for (auto& node : delegate.mNodes)
			{

				nlohmann::json jnode;



				////classify object(imported model), gen object(are made by system), others
				if (node->name.rfind("object", 0) == 0 && node->name.length() > 6) {
					jnode["name"] = "object";
					jnode["object_name"] = node->name.substr(6);
				}
				else if (node->name.rfind("gen_object", 0) == 0 && node->name.length() > 10) {

					jnode["name"] = "gen_object";
					jnode["object_name"] = node->name.substr(10);
				}
				else {
					jnode["name"] = node->name;
				}
				
				jnode["position"] = { node->x, node->y };
				std::vector<ValueVariant> initial = node->get_initial_value();

				jnode["Draw"] = node->Draw_check;

				if(initial.size() > 0)
					jnode["initial_setting"] = serialize_initial_value(initial);


				j["nodes"].push_back(jnode);

			}
			for (auto& link : delegate.mLinks) {
				j["links"].push_back({
					link.mInputNodeIndex,
					link.mInputSlotIndex,
					link.mOutputNodeIndex,
					link.mOutputSlotIndex
					});
			}

			const auto now = std::chrono::system_clock::now();
			std::time_t now_c = std::chrono::system_clock::to_time_t(now);
			std::tm tm{};
#if defined(_WIN32)
			localtime_s(&tm, &now_c);
#else
			localtime_r(&now_c, &tm);
#endif

			std::stringstream ss;
			ss << "node_data/node_data_";
			ss << std::put_time(&tm, "%Y%m%d_%H%M%S");
			ss << ".json";

			std::ofstream(ss.str()) << j.dump(4);

		}

		////node load system 
		if (ImGui::Button("load graph")) 
		{

			namespace fs = std::filesystem;

			std::string latest_file;
			std::time_t latest_time = 0;

			std::regex pattern(R"(node_data_(\d{8}_\d{6})\.json)");


			//// find last node data by data name
			for (const auto& entry : fs::directory_iterator("node_data")) {
				if (entry.is_regular_file()) {
					std::smatch match;
					std::string filename = entry.path().filename().string();

					if (std::regex_match(filename, match, pattern)) {
						std::tm tm = {};
						std::istringstream ss(match[1]);
						ss >> std::get_time(&tm, "%Y%m%d_%H%M%S");

						std::time_t file_time = std::mktime(&tm);
						if (file_time > latest_time) {
							latest_time = file_time;
							latest_file = entry.path().string();
						}
					}
				}
			}

			if (latest_file.empty()) {
				return;
			}

			std::ifstream file(latest_file);
			if (!file.is_open()) {
				return;
			}
			nlohmann::json j;
			file >> j;

			delegate.mNodes.clear();
			delegate.mLinks.clear();
			this->object_count = 0;
			ImVec2 offset;
			if (j.contains("view_position") && j.contains("view_factor")&& j.contains("view_offset"))
			{
				viewState.mPosition = ImVec2(j["view_position"][0], j["view_position"][1]);
				viewState.mFactor = j["view_factor"];

				offset = ImVec2(j["view_offset"][0], j["view_offset"][1]);
			}
			ImVec2 nodeRectangleMin;
			for (const auto& jnode : j["nodes"]) 
			{
				std::string name = jnode["name"];
				ImVec2 position = ImVec2(jnode["position"][0], jnode["position"][1]);

				nodeRectangleMin = position;

				if (name == "object" && jnode.contains("object_name")) {

					std::string object_name = jnode["object_name"];

					delegate.gen_node(nodeRectangleMin, "object", pre_object_map[object_name], this->object_count, pre_shader_map["skin"]);



				}
				else if (name == "gen_object" && jnode.contains("object_name"))
				{
					std::string object_name = jnode["object_name"];

					delegate.gen_node(nodeRectangleMin, (object_name).c_str(), pre_object_map[object_name], this->object_count, pre_shader_map["skin"]);
				}
				else if (name == "animation")
				{
					delegate.gen_node(nodeRectangleMin, (name).c_str(), pre_animation_map["run"]);
				}
				else
				{

					delegate.gen_node(nodeRectangleMin, (name).c_str(), nullptr);



				}

				delegate.mNodes.back()->x = nodeRectangleMin.x;
				delegate.mNodes.back()->y = nodeRectangleMin.y;
				if(jnode.contains("Draw"))
					delegate.mNodes.back()->Draw_check = jnode["Draw"];
				if (jnode.contains("initial_setting")) {
					auto values = deserialize_initial_value(jnode["initial_setting"]);
					delegate.mNodes.back()->initial_setting(values);
				}


			}

			// get link
			for (const auto& jlink : j["links"]) {
				int inNode = jlink[0];
				int inSlot = jlink[1];
				int outNode = jlink[2];
				int outSlot = jlink[3];

				delegate.AddLink(inNode, inSlot, outNode, outSlot);
			}
		}

		GraphEditor::Show(delegate, options, viewState, true, &fit);


		if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(1))
		{

			if (gen_node_click)
				gen_node_click = false;
			else
				gen_node_click = true;
		}
		ImGui::End();
	}




}


nlohmann::json Node_manager::serialize_initial_value(const std::vector<ValueVariant>& values) {
	nlohmann::json json_array = nlohmann::json::array();

	for (const auto& val : values) {
		nlohmann::json item;

		std::visit([&](const auto& v) {
			using T = std::decay_t<decltype(v)>;

			if constexpr (std::is_same_v<T, int>) {
				item["type"] = "int";
				item["value"] = v;
			}
			else if constexpr (std::is_same_v<T, float>) {
				item["type"] = "float";
				item["value"] = v;
			}
			else if constexpr (std::is_same_v<T, glm::vec2>) {
				item["type"] = "vec2";
				item["value"] = { v.x, v.y };
			}
			else if constexpr (std::is_same_v<T, glm::vec3>) {
				item["type"] = "vec3";
				item["value"] = { v.x, v.y, v.z };
			}
			else if constexpr (std::is_same_v<T, glm::vec4>) {
				item["type"] = "vec4";
				item["value"] = { v.x, v.y, v.z, v.w };
			}
			else if constexpr (std::is_same_v<T, glm::mat3>) {
				item["type"] = "mat3";
				for (int i = 0; i < 3; ++i)
					item["value"].push_back({ v[i][0], v[i][1], v[i][2] });
			}
			else if constexpr (std::is_same_v<T, glm::mat4>) {
				item["type"] = "mat4";
				for (int i = 0; i < 4; ++i)
					item["value"].push_back({ v[i][0], v[i][1], v[i][2], v[i][3] });
			}
			else if constexpr (std::is_same_v<T, Quaternion>) {
				item["type"] = "quat";
				item["value"] = { v.x, v.y, v.z, v.w };
			}
			}, val);

		json_array.push_back(item);
	}

	return json_array;
}

std::vector<ValueVariant> Node_manager::deserialize_initial_value(const nlohmann::json& jarray) {
	std::vector<ValueVariant> result;

	for (const auto& item : jarray) {
		std::string type = item["type"];
		const auto& value = item["value"];

		if (type == "int")
			result.emplace_back(value.get<int>());
		else if (type == "float")
			result.emplace_back(value.get<float>());
		else if (type == "vec2")
			result.emplace_back(glm::vec2(value[0], value[1]));
		else if (type == "vec3")
			result.emplace_back(glm::vec3(value[0], value[1], value[2]));
		else if (type == "vec4")
			result.emplace_back(glm::vec4(value[0], value[1], value[2], value[3]));
		else if (type == "mat3") {
			glm::mat3 mat;
			for (int i = 0; i < 3; ++i)
				mat[i] = glm::vec3(value[i][0], value[i][1], value[i][2]);
			result.emplace_back(mat);
		}
		else if (type == "mat4") {
			glm::mat4 mat;
			for (int i = 0; i < 4; ++i)
				mat[i] = glm::vec4(value[i][0], value[i][1], value[i][2], value[i][3]);
			result.emplace_back(mat);
		}
		else if (type == "quat") {
			result.emplace_back(Quaternion(value[0], value[1], value[2], value[3]));
		}
	}

	return result;
}


void Node_manager::imgui_node_property_render(
	const glm::mat4& projection,
	const glm::mat4& view,
	const glm::vec3& camera_position
)
{
	const ImGuiIO& io = ImGui::GetIO();

	if (delegate.sorted_graph.size() > 0 && delegate.node_changed)
	{
		delegate.mNodes[delegate.sorted_graph[0]]->evaluated = false;
		delegate.node_changed = false;
	}


	ImGui::Begin("node property");
	{
		//Run evaluation for all sorted node
		// sorting is already done by topological sort when nodes, links and their value are changed 
		for (auto& graph_index : delegate.sorted_graph)
		{
			delegate.mNodes[graph_index]->evaluate();
		}

		glEnable(GL_DEPTH_TEST);

		// check node's Draw option for all sorted node and real draw 
		for (auto& graph_index : delegate.sorted_graph)
		{
			// if node draw option is ture and evaluated was done
			if (delegate.mNodes[graph_index]->Draw_check && delegate.mNodes[graph_index]->evaluated)
			{
				// if draw is done, draw func return true
				if (delegate.mNodes[graph_index]->Draw(projection, view, camera_position) && delegate.mNodes[graph_index]->mSelected)
				{
					Gizmo gizmo;
					//// check vertex num of object by mouse pointing
					//// if select object node, ver3, mat4  , you can select one point by shift + mousedown
					check_vertex_num(delegate.mNodes[graph_index]->return_vertex_pos());

					if (io.MouseClicked[0] && io.KeyShift)
					{
						debug.set_text("click");
						if (inputctrl_global->check_vertex_num >= 0)
						{
							if (delegate.mNodes[graph_index]->select_vertex_num.size() == 0)
							{
								delegate.mNodes[graph_index]->select_vertex_num.push_back(inputctrl_global->check_vertex_num);
							}
							else
							{
								delegate.mNodes[graph_index]->select_vertex_num.clear();
								delegate.mNodes[graph_index]->select_vertex_num.push_back(inputctrl_global->check_vertex_num);
							}
						}
						gizmo.gizmo_matrix = glm::mat4(1);
						gizmo.origin_matrix.clear();
					}


					// if have select vertex, provide gizmo options
					if (delegate.mNodes[graph_index]->select_vertex_num.size() > 0)
					{
						if (gizmo.origin_matrix.size() == 0)
						{

							gizmo.origin_matrix.push_back(delegate.mNodes[graph_index]->return_transform(delegate.mNodes[graph_index]->select_vertex_num[0]));
							gizmo.gizmo_matrix = gizmo.origin_matrix[0];
							//Currently, the gizmo is provided for a single vertex only,
							//but to support future cases where multiple vertices are selected and manipulated simultaneously using the gizmo, 
							// the (vertex) origin matrix is stored as a vector.

						}


						//calcuate gizmo's translation matrix by user mouse interaction
						if (inputctrl_global->now_gizmo_state == Gizmo_state::TRANSLATE)
						{
							ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection),
								ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::MODE::WORLD, glm::value_ptr(gizmo.gizmo_matrix));
						}
						else if (inputctrl_global->now_gizmo_state == Gizmo_state::ROTATE)
						{
							ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection),
								ImGuizmo::OPERATION::ROTATE, ImGuizmo::MODE::LOCAL, glm::value_ptr(gizmo.gizmo_matrix));
						}





						glm::vec3 gizmo_result = gizmo.gizmo_matrix * glm::vec4(0, 0, 0, 1);
						debug.set_point_group("gizmo result", gizmo_result, glm::vec3(1, 0, 0), 0.35); // draw point that selected vertex position



						if (io.MouseDown[0])
							delegate.mNodes[graph_index]->set_transform(gizmo.gizmo_matrix, delegate.mNodes[graph_index]->select_vertex_num[0]);

					}

				}
				else
				{
					delegate.mNodes[graph_index]->select_vertex_num.clear();

				}
			}

		}

		for (auto& graph_index : delegate.sorted_graph)
		{
			if (delegate.mNodes[graph_index]->mSelected)
				delegate.mNodes[graph_index]->imgui_render();

		}
		const ImGuiIO& io = ImGui::GetIO();


	}
	ImGui::End();

}
