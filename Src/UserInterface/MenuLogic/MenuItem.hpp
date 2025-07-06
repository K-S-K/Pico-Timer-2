#pragma once

#include "../MenuPages/IPage.hpp"

enum class MenuItemType {
    Date,
    Time,
    AlarmTime,
    AlarmConfig,
    Relay,
    System,
    Exit,
    Count
};


class MenuItem
{
public:
        MenuItem(int index, MenuItemType type, const char* name, const char* header)
            : index(index), type(type), name(name), header(header) {}

        bool IsTypeOf(MenuItemType itemType) const {
            return type == itemType;
        }

        const char* GetName() const { return name; }
        const char* GetHeader() const { return header; }

        bool TypeOf(MenuItemType itemType) const {
            return type == itemType;
        }

        int GetIndex() const {
            return index;
        }

        int GetNextItemIndex(int count) const {
            return (index + 1) % count;
        }

        int GetPrevItemIndex(int count) const {
            return (index + count - 1) % count;
        }

        IPage *GetPage() const {
            return page;
        }

        void SetPage(IPage *newPage) {
            page = newPage;
        }

        
    private:
        MenuItemType type;
        const char* name;
        const char* header;
        const int index;
        IPage *page = nullptr;
};
