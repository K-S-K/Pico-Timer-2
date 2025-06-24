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
        MenuItem(int index, MenuItemType type, const char* name)
            : index(index), type(type), name(name) {}

        bool IsTypeOf(MenuItemType itemType) const {
            return type == itemType;
        }
        const char* GetName() const { return name; }

        bool TypeOf(MenuItemType itemType) const {
            return type == itemType;
        }

        int GetIndex() const {
            return index;
        }

        int GetNextItemIndex(int count) const {
            return (static_cast<int>(type) + 1) % count;
        }

        int GetPrevItemIndex(int count) const {
            return (static_cast<int>(type) + count - 1) % count;
        }

        
    private:
        MenuItemType type;
        const char* name;
        const int index;
};
