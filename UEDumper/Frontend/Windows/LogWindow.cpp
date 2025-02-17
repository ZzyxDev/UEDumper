#include "LogWindow.h"



windows::LogWindow::LogWindow()
{

}

void windows::LogWindow::Log(logLevels level, std::string origin, const char* fmt, ...)
{
	if (level < logLevel)
		return;
	char buffer[200];
	auto now = std::chrono::system_clock::now();
	auto now_time_t = std::chrono::system_clock::to_time_t(now);
	std::tm time_info;
	localtime_s(&time_info, &now_time_t);
	// Format time as a string
	std::ostringstream oss;
	oss << std::put_time(&time_info, "%H:%M:%S");
	va_list args;
	va_start(args, fmt);
	vsprintf_s(buffer, fmt, args);
	sprintf_s(buffer, "[%s - %s]: %s", oss.str().c_str(), origin.c_str(), std::string(buffer).c_str());
	logs.push_back(std::string(buffer));
}

int windows::LogWindow::getLogLevel()
{
	return logLevel;
}

std::string windows::LogWindow::getLogLevelName()
{
	return logLevelNames[logLevel];
}

void windows::LogWindow::setLogLevel(int level)
{
	if (level < logLevels::log_MAX)
		logLevel = level;
}

void windows::LogWindow::render()
{
	const int logSize = logs.size();
	ImGui::SetCursorPosY(ImGui::GetWindowSize().y - 310);
	ImGui::BeginChild("Log", ImVec2(ImGui::GetWindowSize().x - 15, 300), true);
	//ImGui::SetWindowSize(ImVec2(1000, 300), ImGuiCond_Once);
	
	if (ImGui::ArrowButton("logselect_btn_fastleft", ImGuiDir_LLeft))
		selectedLogRange = 0;
	ImGui::SameLine();
	if(ImGui::ArrowButton("logselect_btn_left", ImGuiDir_Left) && selectedLogRange >= logRange)
		selectedLogRange -= logRange;
	ImGui::SameLine();
	if(ImGui::ArrowButton("logselect_btn_right", ImGuiDir_Right) && selectedLogRange + logRange < logSize)
		selectedLogRange += logRange;
	ImGui::SameLine();
	if (ImGui::ArrowButton("logselect_btn_fastright", ImGuiDir_RRight))
	{
		while(selectedLogRange + logRange < logSize)
		{
			selectedLogRange += logRange;
		}
	}
	ImGui::SameLine();
	ImGui::Text("Showing Log %d-%d of %d", selectedLogRange, selectedLogRange + logRange - 1 > logSize ? logSize : selectedLogRange + logRange - 1, logSize);
	ImGui::SameLine();
	ImGui::SetCursorPosX(ImGui::GetWindowSize().x - ImGui::CalcTextSize("Showing 999 logs").x - 80);

	if (ImGui::ArrowButton("logrange_btn_left", ImGuiDir_Left) && logRange >= 40)
		logRange -= 20;
	ImGui::SameLine();
	if (ImGui::ArrowButton("logrange_btn_right", ImGuiDir_Right) && logRange < 10000)
		logRange += 20;
	ImGui::SameLine();
	ImGui::Text("Showing %d logs", logRange);
	
	
	if(ImGui::BeginListBox("##loglistbox", ImVec2(ImGui::GetWindowSize().x - 15, ImGui::GetWindowSize().y - 50)))
	{
		for (int i = selectedLogRange; i < logSize && i < selectedLogRange + logRange; i++) {
			const bool is_selected = (selectedLog == i);
			char buf[300] = {0};
			sprintf_s(buf, "%d %s", i, logs[i].c_str());
			if (ImGui::Selectable(buf, is_selected)) {
				selectedLog = i;
			}
			if (is_selected && ImGui::IsItemHovered()) {
				ImGui::BeginTooltip();
				ImGui::Text("%s", logs[i].c_str());
				ImGui::EndTooltip();
			}
		}

		if(oldSize != logs.size())
		{
			ImGui::SetScrollHereY(1.0f);
			oldSize = logs.size();
		}
		
		ImGui::EndListBox();
	}

	ImGui::EndChild();
}

void windows::LogWindow::renderEditPopup()
{
	if(ImGui::Button("Export Log"))
	{
		std::ofstream file(EngineSettings::getWorkingDirectory() / "log.txt");
		for(size_t i = 0; i < logs.size(); i++)
		{
			file << i << " " << logs[i] << std::endl;
		}
		file.close();
		Log(log_2, "LOGWINDOW", "Saved log to %s/%s!", EngineSettings::getWorkingDirectory().string().c_str(), "log.txt");
	}
}
