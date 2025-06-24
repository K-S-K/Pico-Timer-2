#pragma once

enum class MenuItemType {
    Date,
    Time,
    Alarm,
    Relay,
    System,
    Exit,
    Count
};


class MenuItem
{
public:
        MenuItem(MenuItemType type, const char* name)
            : type(type), name(name) {}

        bool IsTypeOf(MenuItemType itemType) const {
            return type == itemType;
        }
        const char* GetName() const { return name; }

        bool TypeOf(MenuItemType itemType) const {
            return type == itemType;
        }

        int GetNextItemIndex() const {
            return (static_cast<int>(type) + 1) % static_cast<int>(MenuItemType::Count);
        }

        int GetPrevItemIndex() const {
            return (static_cast<int>(type) + static_cast<int>(MenuItemType::Count) - 1) % static_cast<int>(MenuItemType::Count);
        }

        
    private:
        MenuItemType type;
        const char* name;
};
