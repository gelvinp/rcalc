Result<> SettingsManager::load() {
    std::optional<fs::path> data_path = get_data_path();
    if (!data_path) {
        return Err(ERR_LOAD_FAILURE, "Cannot locate data directory!");
    }

    fs::path config_path = data_path.value() /= "config.ini";
    if (!fs::exists(config_path)) { return Ok(); }

    if (!fs::is_regular_file(config_path)) { return Err(ERR_LOAD_FAILURE, "Config path does not point to a regular file!"); }

    std::ifstream file;
    file.open(config_path.c_str());
    if (!file.is_open()) { return Err(ERR_LOAD_FAILURE, "Cannot open config file for reading!"); }

    using Parser = std::pair<const char*, bool (*)(SettingsManager&, std::string_view)>;

    constexpr std::array<Parser, 3> parsers = {
        Parser {
            "Colors",
            [](SettingsManager& self, std::string_view option) {
                for (int index = 0; index < MAX_COLORS; ++index) {
                    if (option == COLOR_LABELS[index]) {
                        self.colors = static_cast<ColorScheme>(index);
                        return true;
                    }
                }
                return false;
            }
        },
        Parser {
            "UI_Scale",
            [](SettingsManager& self, std::string_view option) {
                if (option.length() > 4) { return false; }
                float val;
                auto [ptr, ec] = std::from_chars(option.data(), option.data() + option.size(), val, std::chars_format::fixed);
                if (ec == std::errc() && ptr == option.data() + option.size() && val >= 0.5f && val <= 2.0f) {
                    self.ui_scale = val;
                    return true;
                }
                return false;
            }
        },
        Parser {
            "Precision",
            [](SettingsManager& self, std::string_view option) {
                if (option.length() > 2) { return false; }
                int val;
                auto [ptr, ec] = std::from_chars(option.data(), option.data() + option.size(), val, 10);
                if (ec == std::errc() && ptr == option.data() + option.size() && val >= 1 && val <= std::numeric_limits<Real>::max_digits10) {
                    self.precision = val;
                    return true;
                }
                return false;
            }
        }
    };

    std::string line;
    while (std::getline(file, line)) {
        auto colon_it = std::find(line.begin(), line.end(), ':');
        if (colon_it == line.end()) {
            return Err(ERR_LOAD_FAILURE, "Config file is invalid!");
        }

        auto value_begin = std::find_if(colon_it + 1, line.end(), [](char ch) { return !std::isspace(ch); });
        if (value_begin == line.end()) {
            return Err(ERR_LOAD_FAILURE, "Config file is invalid!");
        }

        std::string_view key { line.begin(), colon_it };
        std::string_view value { value_begin, line.end() };

        for (const Parser& parser : parsers) {
            if (parser.first == key) {
                if (parser.second(*this, value)) { continue; }
                return Err(ERR_LOAD_FAILURE, "Config file is invalid!");
            }
        }
    }

    return Ok();
}

Result<> SettingsManager::save() {
    std::optional<fs::path> data_path = get_data_path();
    if (!data_path) {
        return Err(ERR_SAVE_FAILURE, "Cannot locate data directory!");
    }

    fs::path config_path = data_path.value() /= "config.ini";
    if (fs::exists(config_path)) { fs::remove(config_path); }

    std::ofstream file;
    file.open(config_path.c_str());
    if (!file.is_open()) { return Err(ERR_SAVE_FAILURE, "Cannot open config file for writing!"); }

    file << "Colors: " << COLOR_LABELS[colors] << "\n";
    file << "UI_Scale: " << std::fixed << std::setprecision(2) << ui_scale << "\n";
    file << "Precision: " << precision << "\n";

    file.close();

    return Ok();
}