#include "MMLData.h"
#include <algorithm>
#include <cctype>

// Map of color names to RGB values
static const std::map<std::string, Color> colorMap = {
    {"black", Color(0, 0, 0)},
    {"red", Color(255, 0, 0)},
    {"green", Color(0, 128, 0)},
    {"blue", Color(0, 0, 255)},
    {"yellow", Color(255, 255, 0)},
    {"orange", Color(255, 165, 0)},
    {"purple", Color(128, 0, 128)},
    {"cyan", Color(0, 255, 255)},
    {"magenta", Color(255, 0, 255)},
    {"white", Color(255, 255, 255)},
    {"gray", Color(128, 128, 128)},
    {"grey", Color(128, 128, 128)},
    {"brown", Color(165, 42, 42)},
    {"pink", Color(255, 192, 203)},
};

Color Color::FromString(const std::string& colorName) {
    std::string lower = colorName;
    std::transform(lower.begin(), lower.end(), lower.begin(), 
                   [](unsigned char c){ return std::tolower(c); });
    
    auto it = colorMap.find(lower);
    if (it != colorMap.end()) {
        return it->second;
    }
    
    // Default to black if color not found
    return Color(0, 0, 0);
}

void ParticleSimulationData::GetBounds(double& xMin, double& xMax, double& yMin, double& yMax) const {
    // If width and height are explicitly set, use them
    if (width_ > 0 && height_ > 0) {
        xMin = 0;
        xMax = width_;
        yMin = 0;
        yMax = height_;
        return;
    }
    
    // Otherwise calculate from ball positions
    if (balls_.empty() || numSteps_ == 0) {
        xMin = 0; xMax = 100;
        yMin = 0; yMax = 100;
        return;
    }
    
    xMin = 1e30;
    xMax = -1e30;
    yMin = 1e30;
    yMax = -1e30;
    
    for (const auto& ball : balls_) {
        for (int step = 0; step < ball.GetNumSteps(); ++step) {
            auto pos = ball.GetPosition(step);
            xMin = std::min(xMin, pos.x - ball.GetRadius());
            xMax = std::max(xMax, pos.x + ball.GetRadius());
            yMin = std::min(yMin, pos.y - ball.GetRadius());
            yMax = std::max(yMax, pos.y + ball.GetRadius());
        }
    }
}
