cmake_minimum_required(VERSION 3.25)

project(cosmic_bowling_lounge VERSION 1.0.0)

# FIXED: Capitalized 'Geode' so the Linux compiler can find the package
find_package(Geode REQUIRED)

# FIXED: Changed 'add_geode_mod' to 'setup_geode_mod' per Geode standards
setup_geode_mod(
    mateo18023.cosmic_bowling_lounge
    VERSION 1.0.0
    FILES Main.cpp
)
