#ifndef NODE_TEMPLATE_H
#define NODE_TEMPLATE_H

#include <vector>
#include <variant>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm> // std::clamp
#include <iostream>

#include "framemanager.h"

extern Frame_manager frame_manager;

template<typename T>
constexpr bool check_math_type =
std::is_arithmetic_v<T> ||
std::is_same_v<T, glm::vec2> ||
std::is_same_v<T, glm::vec3> ||
std::is_same_v<T, glm::vec4> ||
std::is_same_v<T, glm::mat3> ||
std::is_same_v<T, glm::mat4>;


template<typename T, typename U, typename = void>
struct is_math_compatible : std::false_type {};

template<typename T, typename U>
struct is_math_compatible<T, U, std::void_t<
    decltype(std::declval<T>() + std::declval<U>()),
    decltype(std::declval<T>() - std::declval<U>()),
    decltype(std::declval<T>()* std::declval<U>()),
    decltype(std::declval<T>() / std::declval<U>())
    >> : std::true_type {};


template<typename T>
struct is_std_vector : std::false_type {};

template<typename T, typename A>
struct is_std_vector<std::vector<T, A>> : std::true_type {};

template <typename T2>
struct is_math_vector : std::false_type {};

template <typename U2>
struct is_math_vector<std::vector<U2>> : is_math_compatible<U2, U2> {};


class Node
{
public:
    std::string name;
    int ID_num;
    GraphEditor::TemplateIndex templateIndex;
    float x, y;
    bool mSelected;
    bool Draw_check = false;
    


    std::vector<std::shared_ptr<Node>> input_node;

    bool evaluated = false;

    std::shared_ptr<std::vector<ValueVariant>> value = std::make_shared<std::vector<ValueVariant>>();
    int value_vector_size = 1;

    std::vector<int> select_vertex_num;
    int output_node_num = 0;
    int now_output_count = 0;

    Node(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) :name(name), ID_num(ID_num), templateIndex(templateIndex), x(x), y(y), mSelected(mSelected)
    {

    }

    virtual void evaluate() = 0;
    virtual void update() {};
    virtual void imgui_render();

    
    virtual void initial_setting(const std::vector<ValueVariant>& initial_value)
    {

    }

    virtual std::vector<ValueVariant> get_initial_value()
    {
        std::vector<ValueVariant> new_value;
        return new_value;
    }


    virtual bool Draw(
        const glm::mat4& projection,
        const glm::mat4& view,
        const glm::vec3& camera_position)
    {
        if (value != nullptr)
        {
            for (auto& it : *value)
            {

                std::visit([this, projection, view, camera_position](auto&& arg1) {
                    using T1 = std::decay_t<decltype(arg1)>;

                    if constexpr (std::is_same_v<T1, std::shared_ptr<Object_setting>>) 
                    {
                        arg1->wolrd_object_Draw(projection, view, camera_position);
                        return true;
                    }
                    else
                        return false;
                    }, it);  // 
            }

        }
        else
            return false;



    }

    virtual std::vector<glm::vec3> return_vertex_pos()
    {
        if (value != nullptr)
        {
            auto& it = *value;
            if (it.size() > 0)
            {

                return std::visit([this](auto&& arg1) {
                    using T1 = std::decay_t<decltype(arg1)>;



                    //
                    if constexpr (std::is_same_v<T1, std::shared_ptr<Object_setting>>) {

                        std::vector<glm::vec3> new_pos;
                        for (unsigned int i = 0; i < arg1->object.meshes[0].vertices.size(); i++)
                        {
                            new_pos.emplace_back(arg1->object.meshes[0].vertices[i].Position);
                        }

                        return new_pos;

                    }
                    else
                        return std::vector<glm::vec3>(0);


                    }, it[0]);  // 
            }

        }
        else
            return std::vector<glm::vec3>(0);


    }

    virtual glm::mat4 return_transform
    (
        int vertex_num
    )
    {
        if (value != nullptr)
        {
            auto& it = *value;
            if (it.size() > 0)
            {

                return std::visit([this, vertex_num](auto&& arg1) {
                    using T1 = std::decay_t<decltype(arg1)>;
                    if constexpr (std::is_same_v<T1, std::shared_ptr<Object_setting>>) {

                        glm::mat4 model = glm::mat4(1);
                        return glm::translate(model, arg1->object.meshes[0].vertices[vertex_num].Position);

                    }
                    return glm::mat4(1);



                    }, it[0]);  // 
            }
            return glm::mat4(1);
        }
        else
            return glm::mat4(1);


    }

    virtual void set_transform
    (
        const glm::mat4& new_matrix,
        int vertex_num
    )
    {


    }
protected:

    void base_imgui()
    {
        ImGui::Text((name).c_str());
        if (evaluated)
        {
            ImGui::Text("evaluated!");
        }
        else
        {
            ImGui::Text("not evaluated!");
        }

        ImGui::Text("xy : %3f , %3f", x, y);
        ImGui::Text("vector size : %d", value_vector_size);
        ImGui::Checkbox("Draw", &Draw_check);

        ImGui::Text("output num %d", output_node_num);

    }


    bool Draw_vector_vec3(
        const glm::mat4& projection,
        const glm::mat4& view,
        const glm::vec3& camera_position
    )
    {

        if (this->value != nullptr)
        {

            glm::vec3 temp_vec3_1 = glm::vec3(0.0);
            glm::vec3 temp_vec3_2;
            for (unsigned int i = 0; i < (*this->value).size(); i++)
            {



                std::visit([&](auto&& arg1) {
                    using T1 = std::decay_t<decltype(arg1)>;

                    //
                    if constexpr (std::is_same_v<T1, glm::vec3>) {


                        temp_vec3_2 = arg1;

                        debug.set_point_group((name+"point").c_str(), temp_vec3_2);
                        if (i > 0)
                        {
                            debug.set_line_group((name + "line").c_str(), temp_vec3_1, temp_vec3_2);
                        }


                        temp_vec3_1 = temp_vec3_2;


                        return true;

                    }
                    else
                    {
                        return false;


                    }
                    }, (*this->value)[i]);  // 
            }


        }
        return false;

    }



};

class Node_template_func : public Node
{
public:

    Node_template_func(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node(name, ID_num, templateIndex, x, y, mSelected)
    {


    }






private:

    void evaluate()
    {};


};

class Node_template_value : public Node
{
public:

    Node_template_value(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node(name, ID_num, templateIndex, x, y, mSelected)
    {

    }


    void evaluate() override
    {};

};

class Node_object : public Node
{
public:

    std::shared_ptr<Object_setting> initial_object;
    std::shared_ptr<Object_setting> now_object;

    Node_object(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected,
        std::shared_ptr<Object_setting> object_setting,
        int object_count,
        std::shared_ptr<Shader> shader



    ) : Node(name, ID_num, templateIndex, x, y, mSelected)
    {



        initial_object = std::make_shared<Object_setting>(
            *object_setting,
            object_count,
            shader


        );
        now_object = std::make_shared<Object_setting>(
            *object_setting,
            object_count,
            shader


        );


        auto& it = *value;

        it.push_back(now_object);




    }

    void evaluate() override
    {


        if (input_node.size() >= 1)
        {
            if (input_node[0]->evaluated == true ) 
            {


                if (input_node[0]->value->size() == 1)
                {
                    int curve_point_num;
                    std::vector<glm::vec3> curve_point_vec3;

                    std::visit([&](auto&& arg1) {
                        using T1 = std::decay_t<decltype(arg1)>;
                        if constexpr (std::is_same_v<T1, glm::mat4>) {
                            initial_object->object.position =  glm::vec3(arg1[3]);

                            initial_object->object.scale.x = glm::length(glm::vec3(arg1[0]));  // X축
                            initial_object->object.scale.y = glm::length(glm::vec3(arg1[1]));  // Y축
                            initial_object->object.scale.z = glm::length(glm::vec3(arg1[2]));  // Z축

                            glm::mat3 rotation_matrix;
                            rotation_matrix[0] = glm::vec3(arg1[0]) / initial_object->object.scale.x;
                            rotation_matrix[1] = glm::vec3(arg1[1]) / initial_object->object.scale.y;
                            rotation_matrix[2] = glm::vec3(arg1[2]) / initial_object->object.scale.z;
                            glm::quat rotation = glm::quat_cast(rotation_matrix);


                            
                            initial_object->object.quaternion.w = -rotation.w;
                            initial_object->object.quaternion.x = rotation.x;
                            initial_object->object.quaternion.y = rotation.y;
                            initial_object->object.quaternion.z = rotation.z;


                        }
                        }, (*input_node[0]->value)[0]);  

                }
            }
        }





        *now_object = *initial_object;
        evaluated = true;
    };

    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {
            ImGui::Text((name).c_str());

            ImGui::Checkbox("Draw", &Draw_check);



        }
        ImGui::EndChild();



    }


    void set_transform
    (
        const glm::mat4& new_matrix,
        int vertex_num
    ) override
    {

        glm::vec3 new_pos = new_matrix * glm::vec4(0, 0, 0, 1);
        initial_object->object.meshes[0].vertices[vertex_num].Position = new_pos;

        evaluated = false;


    }



};


// Based on "Camera class" from LearnOpenGL by Joey de Vries
// https://learnopengl.com/Getting-started/Camera
// Licensed under CC BY 4.0 — https://creativecommons.org/licenses/by/4.0/
// Twitter: https://twitter.com/JoeyDeVriez
class Node_animation : public Node
{
public:
    std::shared_ptr<Animation> initial_value;
    std::vector<glm::mat4> bone_matrices;

    int frame = 0;
    float float_t = 0;
    Node_animation(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected,
        std::shared_ptr<Animation> animation



    ) : Node(name, ID_num, templateIndex, x, y, mSelected)
    {

        bone_matrices.reserve(animation->m_BoneInfoMap.size());
        for (int i = 0; i < animation->m_BoneInfoMap.size(); i++)
            bone_matrices.push_back(glm::mat4(1.0f));


 
        initial_value = std::make_shared<Animation>(
            *animation
        );


    }

    void evaluate() override
    {
        if (input_node.size() > 0)
        {
            this->value = input_node[0]->value;
            frame = frame_manager.now_frame;
            std::visit([this](auto&& arg1) {
                using T1 = std::decay_t<decltype(arg1)>;
                if constexpr (std::is_same_v<T1, std::shared_ptr<Object_setting>>) 
                {
                    real_act(arg1->object);
                    arg1->object.final_bone_matrix.clear();
                 
                    for (auto& it : bone_matrices)
                    {
                        arg1->object.final_bone_matrix.emplace_back(it);
                    }
                    evaluated = true;
                }

            }, (*this->value)[0]);  // 

            input_node[0]->evaluated = false;
        }
        else
        {
            this->value = nullptr;
        }
    }

    void real_act(Model& model)
    {
        if (this->initial_value == nullptr)
        {
            debug.set_text("error : don't have animation in this model", 0.1);
        }
        else
        {
            CalculateBoneTransform(this->initial_value, bone_matrices, &this->initial_value->GetRootNode(), glm::mat4(1.0f));


        }

    }

    void CalculateBoneTransform(
        std::shared_ptr<Animation> animation,
        std::vector<glm::mat4>& m_FinalBoneMatrices,
        const AssimpNodeData* node,
        glm::mat4 parentTransform
    )
    {
        std::string nodeName = node->name;
        glm::mat4 nodeTransform = node->transformation;

        Bone2* Bone = animation->FindBone(nodeName);

        float animation_time = frame * 2.0;


        static float animation_dt_temp = 5.50;



        debug.set_parameter("animation dt", animation_dt_temp, 1, 100);



        animation_time *= animation_dt_temp;
        animation_time = std::fmod(animation_time, animation->GetDuration());



        if (Bone)
        {

            Bone->Update(animation_time);
            nodeTransform = Bone->GetLocalTransform();
        }


        glm::mat4 globalTransformation = parentTransform * nodeTransform;

        auto boneInfoMap = animation->GetBoneIDMap();
        if (boneInfoMap.find(nodeName) != boneInfoMap.end())
        {
            int index = boneInfoMap[nodeName].id;
            static float temp_scale = 1;
            glm::mat4 offset = boneInfoMap[nodeName].offset * temp_scale;


            static float arm_angle_x = 90;
            static float arm_angle_y = 0;
            static float arm_angle_z = 90;
            debug.set_parameter("arm angle x", arm_angle_x, -180, 180.0);
            debug.set_parameter("arm angle y", arm_angle_y, -180, 180.0);
            debug.set_parameter("arm angle z", arm_angle_z, -180, 180.0);

            offset = glm::scale(offset, glm::vec3(temp_scale));

            m_FinalBoneMatrices[index] = globalTransformation * offset;



        }

        for (int i = 0; i < node->childrenCount; i++)
            CalculateBoneTransform(animation, m_FinalBoneMatrices, &node->children[i], globalTransformation);
    }


    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {
            base_imgui();
            if (ImGui::SliderInt("frame", &frame, 0, 200))
            {
                evaluated = false;
            }

            ImGui::SliderFloat("0~1 t", &float_t, 0, 1);



        }
        ImGui::EndChild();



    }

};




class Node_value_float : public Node_template_value
{
public:

    std::vector<float> initial_value = std::vector<float>(1,0.0f);

    Node_value_float(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_value(name, ID_num, templateIndex, x, y, mSelected)
    {

        value->push_back(initial_value[0]);
    }

    void initial_setting(const std::vector<ValueVariant>& initial_value) override
    {

        this->value_vector_size = initial_value.size();

        std::vector<float>().swap(this->initial_value);


        for (auto& it : initial_value)
        {
            std::visit([this](auto&& arg1)
                {
                    using T1 = std::decay_t<decltype(arg1)>;

                    if constexpr (std::is_same_v<T1, float>)
                    {

                        this->initial_value.emplace_back(arg1);

                    }

            }, it);

        }



    }
    std::vector<ValueVariant> get_initial_value() override
    {
        std::vector<ValueVariant> this_value;

        for (auto& it : initial_value)
        {
            this_value.emplace_back(it);
        }

        return this_value;
    }



    void evaluate() override
    {

        value->clear();
        for (auto& v : initial_value)
        {

            value->emplace_back(v);
        }

        evaluated = true;

    }

private:
    //void imgui_node();

    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {
            ImGui::Text((name).c_str());

            base_imgui();
            bool changed = false;
            
            
            
            changed |= ImGui::InputInt((name + "size").c_str(), &value_vector_size, 1, 10);

            if (value_vector_size < 1)
            {
                value_vector_size = 1;
            }

            if (initial_value.size() != value_vector_size)
            {
                initial_value.resize(value_vector_size);
            }


            for (unsigned int i = 0; i < initial_value.size(); i++)
            {
                changed = ImGui::InputFloat((name + std::to_string(i)).c_str(), &initial_value[i], 0.1f, 1.0f, "%.3f");
            }

            if (changed)
            {
                evaluated = false;
            }

        }
        ImGui::EndChild();


    }


};




class Node_value_int : public Node_template_value
{
public:
    std::vector<int> initial_value = std::vector<int>(1,0);
    Node_value_int(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_value(name, ID_num, templateIndex, x, y, mSelected)
    {
        value->push_back(initial_value[0]);
    }

    void initial_setting(const std::vector<ValueVariant>& initial_value) override
    {

        this->value_vector_size = initial_value.size();

        std::vector<int>().swap(this->initial_value);


        for (auto& it : initial_value)
        {
            std::visit([this](auto&& arg1)
                {
                    using T1 = std::decay_t<decltype(arg1)>;

                    if constexpr (std::is_same_v<T1, int>)
                    {

                        this->initial_value.emplace_back(arg1);

                    }

                }, it);

        }



    }
    std::vector<ValueVariant> get_initial_value() override
    {
        std::vector<ValueVariant> this_value;

        for (auto& it : initial_value)
        {
            this_value.emplace_back(it);
        }

        return this_value;
    }
    void evaluate() override
    {

        value->clear();
        for (auto& v : initial_value)
        {

            value->emplace_back(v);
        }

        evaluated = true;

    }

private:
    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {
            ImGui::Text((name).c_str());

            bool changed = false;


            changed |= ImGui::InputInt((name + "size").c_str(), &value_vector_size, 1, 10);

            if (value_vector_size < 1)
            {
                value_vector_size = 1;
            }

            if (initial_value.size() != value_vector_size)
            {
                initial_value.resize(value_vector_size);
            }



            for (unsigned int i = 0; i < initial_value.size(); i++)
            {
                changed = ImGui::InputInt((name + std::to_string(i)).c_str(), &initial_value[i], 1,10);
            }

            if (changed)
            {
                evaluated = false;
            }


        }
        ImGui::EndChild();


    }


};




class Node_value_vec3 : public Node_template_value
{
public:
    std::vector<glm::vec3> initial_value = std::vector<glm::vec3>(1, glm::vec3(0.0));
    Node_value_vec3(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_value(name, ID_num, templateIndex, x, y, mSelected)
    {
        value->push_back(initial_value[0]);
    }
    void evaluate() override
    {

        value->clear();
        for (auto& v : initial_value)
        {

            value->emplace_back(v);
        }
        evaluated = true;
    }
    void initial_setting(const std::vector<ValueVariant>& initial_value) override
    {

        this->value_vector_size = initial_value.size();

        std::vector<glm::vec3>().swap(this->initial_value);


        for (auto& it : initial_value)
        {
            std::visit([this](auto&& arg1)
                {
                    using T1 = std::decay_t<decltype(arg1)>;

                    if constexpr (std::is_same_v<T1, glm::vec3>)
                    {

                        this->initial_value.emplace_back(arg1);

                    }

                }, it);

        }



    }
    std::vector<ValueVariant> get_initial_value() override
    {
        std::vector<ValueVariant> this_value;

        for (auto& it : initial_value)
        {
            this_value.emplace_back(it);
        }

        return this_value;
    }


    bool Draw(
        const glm::mat4& projection,
        const glm::mat4& view,
        const glm::vec3& camera_position) override
    {

        for (unsigned int i = 0; i < value_vector_size; i++)
        {
            debug.set_point_group("vec3", initial_value[i]);
        }
        

        return true;

    }
    std::vector<glm::vec3> return_vertex_pos() override
    {

        return initial_value;


    }
    glm::mat4 return_transform(int vertex_num) override
    {
        glm::mat4 new_mat = glm::mat4(1);
        new_mat = glm::translate(new_mat, initial_value[vertex_num]);

        return new_mat;


    }
    void set_transform(const glm::mat4& new_matrix, int vertex_num) override
    {

        glm::vec3 new_pos = new_matrix * glm::vec4(0, 0, 0, 1);
        initial_value[vertex_num] = new_pos;

        evaluated = false;


    }

private:
    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {
            base_imgui();



            bool changed = false;


            changed |= ImGui::InputInt((name + "size").c_str(), &value_vector_size, 1, 10);

            if (value_vector_size < 1)
            {
                value_vector_size = 1;
            }

            if (initial_value.size() != value_vector_size)
            {
                initial_value.resize(value_vector_size);
            }

            

            for (unsigned int i = 0; i < initial_value.size(); i++)
            {
                changed |= ImGui::InputFloat(("x" + std::to_string(i) + "##" + name).c_str(), &initial_value[i][0], 0.01f, 1.0f, "%.3f");
                changed |= ImGui::InputFloat(("y" + std::to_string(i) + "##" + name).c_str(), &initial_value[i][1], 0.01f, 1.0f, "%.3f");
                changed |= ImGui::InputFloat(("z" + std::to_string(i) + "##" + name).c_str(), &initial_value[i][2], 0.01f, 1.0f, "%.3f");
                ImGui::Spacing();
                ImGui::Spacing();
            }

            if (changed)
            {
                evaluated = false;
            }


        }
        ImGui::EndChild();


    }


};



class Node_value_mat4 : public Node_template_value
{
public:
    std::vector<glm::mat4> initial_value = std::vector<glm::mat4>(1, glm::mat4(1.0));
    Node_value_mat4(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_value(name, ID_num, templateIndex, x, y, mSelected)
    {
        value->push_back(initial_value[0]);
    }
    void evaluate() override
    {

        value->clear();
        for (auto& v : initial_value)
        {

            value->emplace_back(v);
        }
        evaluated = true;
    }
    void initial_setting(const std::vector<ValueVariant>& initial_value) override
    {

        this->value_vector_size = initial_value.size();

        std::vector<glm::mat4>().swap(this->initial_value);


        for (auto& it : initial_value)
        {
            std::visit([this](auto&& arg1)
                {
                    using T1 = std::decay_t<decltype(arg1)>;

                    if constexpr (std::is_same_v<T1, glm::mat4>)
                    {

                        this->initial_value.emplace_back(arg1);

                    }

                }, it);

        }



    }
    std::vector<ValueVariant> get_initial_value() override
    {
        std::vector<ValueVariant> this_value;

        for (auto& it : initial_value)
        {
            this_value.emplace_back(it);
        }

        return this_value;
    }


    bool Draw(
        const glm::mat4& projection,
        const glm::mat4& view,
        const glm::vec3& camera_position) override
    {

        for (unsigned int i = 0; i < value_vector_size; i++)
        {
            debug.set_axis("mat4", initial_value[i]);
        }


        return true;

    }
    std::vector<glm::vec3> return_vertex_pos() override
    {
        std::vector<glm::vec3> now_pos_list;
        for (auto& it : initial_value)
        {
            glm::vec3 now_pos = glm::vec3(it[3]);
            now_pos_list.emplace_back(now_pos);
        }

        return now_pos_list;


    }
    glm::mat4 return_transform(int vertex_num) override
    {
        return initial_value[vertex_num];
    }
    void set_transform(const glm::mat4& new_matrix, int vertex_num) override
    {

        initial_value[vertex_num] = new_matrix;

        evaluated = false;


    }

private:
    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {
            base_imgui();



            bool changed = false;


            changed |= ImGui::InputInt((name + "size").c_str(), &value_vector_size, 1, 10);

            if (value_vector_size <= 1)
            {
                value_vector_size = 1;
            }

            if (initial_value.size() != value_vector_size)
            {
                initial_value.resize(value_vector_size);
                initial_value.back() = glm::mat4(1.0f);
            }



            if (changed)
            {
                evaluated = false;
            }


        }
        ImGui::EndChild();


    }


};












/*
class Node_value_vector_vec3 : public Node_template_value
{
public:
    std::vector<glm::vec3> initial_value = std::vector<glm::vec3>(1,glm::vec3(0.0));
    int vector_size = 1;
    
    Node_value_vector_vec3(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_value(name, ID_num, templateIndex, x, y, mSelected)
    {

        this->now_value = initial_value;
        value = std::make_shared<ValueVariant>(this->now_value);
    }

    void initial_setting(const ValueVariant& initial_value) override
    {

        std::visit([this](auto&& arg1) {
            using T1 = std::decay_t<decltype(arg1)>;

            //
            if constexpr (std::is_same_v<T1, std::vector<glm::vec3>>) {

                vector_size = arg1.size();
                this->initial_value = arg1;
            }

        }, initial_value);  // 
        
       

    }
    void update() override
    {

    }

    void evaluate() override
    {

        //initial_value.assign(vector_size, initial_vec3);

        *value = initial_value;
        evaluated = true;

    }

private:
    //void imgui_node();

    void imgui_render()
    {
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {
            ImGui::Text((name).c_str());


            bool changed = false;
            
            changed |= ImGui::SliderInt((name + "size").c_str(), &vector_size, 1, 100);

            if (changed)
            {
                initial_value.assign(vector_size, glm::vec3(0.0));
            }



            for (unsigned int i = 0; i < vector_size; i++)
            {
                ImGui::BeginChild((std::to_string(i)).c_str(), ImVec2(450, 100));
                {

                    changed |= ImGui::InputFloat((name + "x##" +std::to_string(i)).c_str(), &initial_value[i][0], 0.1f, 10.0f, "%.3f");
                    changed |= ImGui::InputFloat((name + "y##" + std::to_string(i)).c_str(), &initial_value[i][1], 0.1f, 10.0f, "%.3f");
                    changed |= ImGui::InputFloat((name + "z##" + std::to_string(i)).c_str(), &initial_value[i][2], 0.1f, 10.0f, "%.3f");
                }
                ImGui::EndChild();
            }


            if (changed)
            {
                evaluated = false;
            }

            /*
            for (int i = 0; i < vector_size; i++)
            {

                changed |= ImGui::InputFloat((name + "x").c_str(), &initial_value[i][0], 0.1f, 1.0f, "%.3f");
                changed |= ImGui::InputFloat((name + "y").c_str(), &initial_value[i][1], 0.1f, 1.0f, "%.3f");
                changed |= ImGui::InputFloat((name + "z").c_str(), &initial_value[i][2], 0.1f, 1.0f, "%.3f");



            }
            

            if (changed)
            {
                evaluated = false;
            }

        }
        ImGui::EndChild();


    }

};
*/




class Node_func_add : public Node_template_func
{
public:
    Node_func_add(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_func(name, ID_num, templateIndex, x, y, mSelected)
    {

    }

    void evaluate() override
    {

        if (input_node.size() >= 2) {

            this->value = input_node[0]->value;

            auto& vec1 = *input_node[0]->value;
            auto& vec2 = *input_node[1]->value;

            for (size_t i = 0; i < std::min(vec1.size() , vec2.size()); i++)
            {

                std::visit([this,i](auto&& arg1, auto&& arg2) {
                    using T1 = std::decay_t<decltype(arg1)>;
                    using T2 = std::decay_t<decltype(arg2)>;

                    //
                    if constexpr (is_math_compatible<T1, T2>::value) {
                        (*this->value)[i] = arg1 + arg2;  // 
                    }
                    }, vec1[i], vec2[i]);

            }



            evaluated = true;

        }

 

    }

private:
};

class Node_func_add_all : public Node_template_func
{
public:
    Node_func_add_all(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_func(name, ID_num, templateIndex, x, y, mSelected)
    {

    }

    void evaluate() override
    {

        if (input_node.size() >= 2) {

            this->value = input_node[0]->value;

            auto& vec1 = *input_node[0]->value;
            auto& vec2 = *input_node[1]->value;

            for (size_t i = 0; i < std::min(vec1.size(), vec2.size()); i++)
            {

                std::visit([this, i](auto&& arg1, auto&& arg2) {
                    using T1 = std::decay_t<decltype(arg1)>;
                    using T2 = std::decay_t<decltype(arg2)>;

                    //
                    if constexpr (is_math_compatible<T1, T2>::value) {
                        (*this->value)[i] = arg1 + arg2;  // 
                    }
                    }, vec1[i], vec2[i]);

            }



            evaluated = true;

        }



    }

private:
};



class Node_func_subtract : public Node_template_func
{
public:
    Node_func_subtract(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_func(name, ID_num, templateIndex, x, y, mSelected)
    {

    }

    void evaluate() override
    {


        if (input_node.size() >= 2) {

            this->value = input_node[0]->value;

            auto& vec1 = *input_node[0]->value;
            auto& vec2 = *input_node[1]->value;

            for (size_t i = 0; i < std::min(vec1.size(), vec2.size()); i++)
            {

                std::visit([this, i](auto&& arg1, auto&& arg2) {
                    using T1 = std::decay_t<decltype(arg1)>;
                    using T2 = std::decay_t<decltype(arg2)>;

                    //
                    if constexpr (is_math_compatible<T1, T2>::value) {
                        (*this->value)[i] = arg1 - arg2;  // 
                    }
                    }, vec1[i], vec2[i]);

            }



            evaluated = true;

        }


    }




private:


};




class Node_func_divide : public Node_template_func
{
public:
    Node_func_divide(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_func(name, ID_num, templateIndex, x, y, mSelected)
    {

    }

    void evaluate() override
    {

        if (input_node.size() >= 2) {

            this->value = input_node[0]->value;

            auto& vec1 = *input_node[0]->value;
            auto& vec2 = *input_node[1]->value;

            for (size_t i = 0; i < std::min(vec1.size(), vec2.size()); i++)
            {

                std::visit([this, i](auto&& arg1, auto&& arg2) {
                    using T1 = std::decay_t<decltype(arg1)>;
                    using T2 = std::decay_t<decltype(arg2)>;

                    //
                    if constexpr (is_math_compatible<T1, T2>::value) {
                        (*this->value)[i] = arg1 / arg2;  // 
                    }
                    }, vec1[i], vec2[i]);

            }



            evaluated = true;

        }



    }




private:


};




class Node_func_multiply : public Node_template_func
{
public:
    Node_func_multiply(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_func(name, ID_num, templateIndex, x, y, mSelected)
    {

    }

    void evaluate() override
    {

        if (input_node.size() >= 2) {

            this->value = input_node[0]->value;

            auto& vec1 = *input_node[0]->value;
            auto& vec2 = *input_node[1]->value;

            for (size_t i = 0; i < std::min(vec1.size(), vec2.size()); i++)
            {

                std::visit([this, i](auto&& arg1, auto&& arg2) {
                    using T1 = std::decay_t<decltype(arg1)>;
                    using T2 = std::decay_t<decltype(arg2)>;

                    //
                    if constexpr (is_math_compatible<T1, T2>::value) {
                        (*this->value)[i] = arg1 * arg2;  // 
                    }
                    }, vec1[i], vec2[i]);

            }



            evaluated = true;

        }


    }




private:


};

/*
class Node_func_vector_multiply : public Node_template_func
{
public:
    Node_func_vector_multiply(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_func(name, ID_num, templateIndex, x, y, mSelected)
    {

    }

    void evaluate() override
    {

        if (input_node.size() >= 2) {

            this->value = input_node[0]->value;

            std::visit([this](auto&& arg1, auto&& arg2) {
                using T1 = std::decay_t<decltype(arg1)>;
                using T2 = std::decay_t<decltype(arg2)>;

                //
                if constexpr (is_math_compatible<T1, T2>::value) {
                    *this->value = arg1 * arg2;  // 
                }
                else if constexpr ()
                {


                }
                }, *input_node[0]->value, *input_node[1]->value);  // 
        }

        evaluated = true;
    }




private:


};

*/


class Node_func_result : public Node_template_func
{
public:
    Node_func_result(
        const std::string& name,
        int ID_num,
        GraphEditor::TemplateIndex templateIndex,
        float x,
        float y,
        bool mSelected

    ) : Node_template_func(name, ID_num, templateIndex, x, y, mSelected)
    {



    }

    void evaluate() override
    {
        if (input_node.size() > 0)
            this->value = input_node[0]->value;

        evaluated = true;
    }

private:
    void imgui_render() override
    {
        ImGui::Text((name).c_str());
        ImGui::BeginChild((name).c_str(), ImVec2(450, 200), ImGuiWindowFlags_NoTitleBar);
        {
            for (unsigned int i = 0; i < input_node.size(); i++)
            {
                ImGui::Text((input_node[i]->name).c_str());
            }           


            if (this->value != nullptr)
            {
                for (auto& it : *this->value)
                {
                    std::visit([](auto&& val)
                        {
                            using T = std::decay_t<decltype(val)>;
                            if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>)
                            {
                                ImGui::Text("value : %.3f", val);
                            }
                            else if constexpr (std::is_same_v<T, int>)
                            {
                                ImGui::Text("value : %d", val);
                            }
                            else if constexpr (std::is_same_v<T, std::string>)
                            {
                                ImGui::Text("value : %s", val.c_str());
                            }
                            else if constexpr (std::is_same_v<T, glm::vec3>)
                            {
                                ImGui::Text("value: %s", glm::to_string(val).c_str());
                            }

                            else
                            {
                                ImGui::Text("value : (unsupported type)");
                            }
                        }, it);

                }


            }




        }
        ImGui::EndChild();

    }


};






class end_node : public Node
{
public:



private:


};
















#endif // 
