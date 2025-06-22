#pragma once

enum class InputElementType {
    Cancel,
    Apply,
    Data,
};

enum class InputElementMode  
{
    Bypass,
    Select,
    Modify,
};

class InputElement
{
    public:
    InputElement(IDisplay* display, int row, int col, InputElementType type)
    : display(display), row(row), col(col), type(type) {}

    void Render(InputElementMode  mode)
    {

        display->ShowText(3, 0, hintClear); // Clear previous text

        const char* hint = 
            (type == InputElementType::Cancel) ? "Cancel" :
            (type == InputElementType::Apply) ? "Apply" :
            (type == InputElementType::Data) ? 
            
            (mode == InputElementMode    ::Bypass) ? hintClear :
            (mode == InputElementMode    ::Select) ? hintSelect :
            (mode == InputElementMode    ::Modify) ? hintModify : "" : "";

        if(type == InputElementType::Data) {
            const char* label = 
                (mode == InputElementMode    ::Bypass) ? " " :
                (mode == InputElementMode    ::Select) ? "^" :
                (mode == InputElementMode    ::Modify) ? ">" : "?" ;

            display->ShowText(row, col, label);
        }

        display->ShowText(3, 0, hint);
    }

    InputElementType type;

    private:
    int row;
    int col;
    IDisplay* display;

    const char* hintSelect = "Select";
    const char* hintModify = "Modify";
    const char* hintClear =  "      ";
};
