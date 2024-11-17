#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>

// 去除字符串两端的空白字符
static std::string strip(const std::string& str) {
	auto start = std::find_if_not(str.begin(), str.end(), [](char c) {
		return std::isspace(static_cast<unsigned char>(c));
		});

	auto end = std::find_if_not(str.rbegin(), str.rend(), [](char c) {
		return std::isspace(static_cast<unsigned char>(c));
		}).base();

	return (start < end) ? std::string(start, end) : std::string();
}

static std::string removeExtraNewlines(const std::string& str) {
	std::string result;
	bool lastWasNewline = false;
	for (size_t i = 0; i < str.size(); ++i) {
		char c = str[i];
		if (c == '\n') {
			if (lastWasNewline) continue; // 忽略连续的换行
			lastWasNewline = true;
		}
		else {
			lastWasNewline = false;
		}
		result.push_back(c);
	}
	return result;
}

int main(int argc, char* argv[]) {
	namespace fs = std::filesystem;

	fs::path filePath{ "sdvx@asphyxia.db" };

	if (argc > 1) {
		filePath = argv[1];
	}

	if (not fs::exists(filePath)) {
		std::cerr << "找不到存档文件" << filePath << "呢,检查一下当前目录是否存在这个文件叭" << std::endl;
		std::system("pause");
		return 1;
	}

	// 打开文件并读取内容到 std::string
	std::ifstream file(filePath, std::ios::in | std::ios::binary);
	if (!file) {
		std::cerr << "无法打开文件" << filePath << "进行读取" << std::endl;
		std::system("pause");
		return 1;
	}

	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();

	std::ofstream outfile(filePath.string().append(".bak"), std::ios::out | std::ios::binary | std::ios::trunc);
	if (!outfile) {
		std::cerr << "无法创建文件" << filePath << std::endl;
		std::system("pause");
		return 1; // 返回非零值表示错误
	}

	std::regex patternStep1(R"(^\{"collection":"course","sid":(.*)\}\}$)"), patternStep2(R"(\}\s*\n(?:\s*\n){1,}\{)");
	std::string fixReplaceStr1{""}, fixReplaceStr2{"}\n{"};

	outfile << content;
	outfile.close();
	
	auto fixCtx1{ std::regex_replace(content, patternStep1, fixReplaceStr1) };

	// std::cout << fixCtx1 << std::endl;

	auto fixCtx2{ removeExtraNewlines(fixCtx1) };

	std::string result{ strip(fixCtx2) };

	result += "\n";

	std::ofstream resfile(filePath, std::ios::out | std::ios::binary | std::ios::trunc);
	if (!resfile) {
		std::cerr << "无法创建文件" << filePath << std::endl;
		std::system("pause");
		return 1; // 返回非零值表示错误
	}

	resfile << result;
	resfile.close();

	std::cout << "修复完成" << std::endl;
	std::system("pause");

	return 0;
}
