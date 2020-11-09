#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <cstdio>
#include <unordered_set>
#include <cassert>
#include <experimental/filesystem>
#include <iostream>

//TODO add txlib

// Create the main window
const sf::Vector2u MainWindow_size(1200, 800);
sf::RenderWindow MainWindow(sf::VideoMode(MainWindow_size.x, MainWindow_size.y), "SFML window", sf::Style::Default);

const unsigned int FontSize = 20;

#define ASSERT(CONDITION) do {    \
            if (!(CONDITION)) {   \
                printf("Some unexpected error occur!!!\n"); \
                printf("%s->%s:%d: " #CONDITION, __FILE__, __PRETTY_FUNCTION__, __LINE__); \
                exit(0);        \
            }                   \
        } while (0)
////////////////////////////////// Miscellaneous sprites ///////////////////////////////////////////////
struct ButtonType
{
    sf::Vector2f icords;  ///< image cords
    sf::Vector2u isize;   ///< image size
    sf::Vector2u icenter; ///< image center. For printing text on button and plot starting
};

const ButtonType RedButton   {sf::Vector2f(36, 68),   sf::Vector2u(205-36, 142-68),   sf::Vector2u(28, 18)};
const ButtonType BlueButton  {sf::Vector2f(247, 270), sf::Vector2u(423-247, 349-270), sf::Vector2u(28, 18)};
const ButtonType GreenButton {sf::Vector2f(457, 282), sf::Vector2u(548-457, 361-282), sf::Vector2u(28, 18)};
/////////////////////////////////////////////////////////////////////////////////////////////////////////


class AWindow
{
    sf::Vector2f cords_;
    sf::Vector2u size_;
    bool         isHidden_;
    bool         isFocus_;
    sf::Font     font_;
    sf::Text     text_;

public:
    AWindow(sf::Vector2f cords, sf::Vector2u size, const std::string& text = " ") :
            cords_      (cords),
            size_       (size),
            isHidden_   (false),
            isFocus_    (false),
            font_       (sf::Font()),
            text_       (sf::Text(text, font_, FontSize))
    {}
    virtual ~AWindow()                 = default;
    AWindow (const AWindow&)           = default;
    AWindow (AWindow&& deleted_window) :
        cords_      (std::move(deleted_window.cords_)),
        size_       (std::move(deleted_window.size_)),
        isHidden_   (deleted_window.isHidden_),
        isFocus_    (deleted_window.isFocus_),
        font_       (std::move(deleted_window.font_)),
        text_       (std::move(deleted_window.text_))
    {

    }
    AWindow& operator=(const AWindow&) = default;
    AWindow& operator=(AWindow&& deleted_window)
    {
        cords_    = std::move(deleted_window.cords_);
        size_     = std::move(deleted_window.size_);
        isHidden_ = deleted_window.isHidden_;
        isFocus_  = deleted_window.isFocus_;
        font_     = std::move(deleted_window.font_);
        text_     = std::move(deleted_window.text_);

        return *this;
    }

    sf::Vector2u getSize() const noexcept
    {
        return size_;
    }
    bool setSize(sf::Vector2u newSize)
    {
        size_ = newSize;
        return true;
    }

    sf::Vector2f getCords() const noexcept
    {
        return cords_;
    }
    bool setCords(sf::Vector2f newCords)
    {
        cords_ = newCords;
        return true;
    }

    bool getIsHidden() const noexcept
    {
        return isHidden_;
    }
    bool setIsHidden(bool newIsHidden)
    {
        isHidden_ = newIsHidden;
        return true;
    }

    bool getIsFocus() const noexcept
    {
        return isFocus_;
    }
    bool setIsFocus(bool newIsFocus)
    {
        isFocus_ = newIsFocus;
        return true;
    }

    sf::Font getFont() const noexcept
    {
        return font_;
    }
    bool setFont(const sf::Font& newFont)
    {
        font_ = newFont;
        text_.setFont(font_);
        return 1;
    }
    bool setFont(sf::Font&& newFont)
    {
        font_ = std::move(newFont);
        text_.setFont(font_);
        return 1;
    }

    sf::Text getText() const noexcept
    {
        return text_;
    }
    bool setText(const sf::Text& newText)
    {
        text_ = newText;
        text_.setFont(font_);
        text_.setCharacterSize(FontSize);
        return 1;
    }
    bool setText(sf::Text&& newText)
    {
        text_ = std::move(newText);
        text_.setFont(font_);
        text_.setCharacterSize(FontSize);
        return 1;
    }
    bool setString(const std::string& new_string)
    {
        text_.setString(new_string);
        return 1;
    }

    virtual void onMouse() {}
    virtual void outMouse() {}
    virtual void onClick() {}
    virtual void onRelease() {}
    virtual void draw() {}
};

class AButton : public AWindow
{
public:
    AButton(sf::Vector2f cords, sf::Vector2u size, const std::string& text = " ") :
        AWindow(cords,
                size,
                text)
    {
        sf::Font tempFont;
        sf::Text tempText;

        ASSERT(tempFont.loadFromFile("../data/9404.ttf"));
        tempText.setPosition(getCords().x, getCords().y);

        tempText.setFillColor(sf::Color(sf::Color::Black));
        tempText.setString(text);

        setFont(tempFont);
        setText(tempText);
    }
    virtual ~AButton()                 = default;
    AButton (const AButton&)           = default;
    AButton (AButton&&)                = default;
    AButton& operator=(const AButton&) = default;
    AButton& operator=(AButton&&)      = default;

};

class TextureButton : public AButton
{
    const sf::Color OnMouseColor = sf::Color(230, 230, 230);
    const sf::Color OnClickColor = sf::Color(160, 160, 160);

    ButtonType   buttonType_;
    sf::Texture  texture_;
    sf::Sprite   sprite_;

public:
    TextureButton(ButtonType buttonType, sf::Vector2f cords, const std::string& text = " ") :
            AButton(cords,
               sf::Vector2u(buttonType.isize.x, buttonType.isize.y), //button size == image size
               text),
            buttonType_  (buttonType),
            texture_     (sf::Texture()),
            sprite_      (sf::Sprite())
    {
        ASSERT(texture_.loadFromFile("../data/button.png", sf::IntRect(static_cast<int>(buttonType_.icords.x),
                                                                       static_cast<int>(buttonType_.icords.y),
                                                                       static_cast<int>(buttonType_.isize.x),
                                                                       static_cast<int>(buttonType_.isize.y))));
        texture_.setSmooth(true);
        sprite_.setTexture(texture_);
        sprite_.setPosition(getCords());

        sf::Text tempText = getText();
        tempText.setPosition(getCords().x + static_cast<float>(buttonType_.icenter.x),
                             getCords().y + static_cast<float>(buttonType_.icenter.y));
        setText(tempText);
    }
    virtual ~TextureButton()                       = default;
    TextureButton (const TextureButton&)           = delete;
    TextureButton (TextureButton&&)                = delete;
    TextureButton& operator=(const TextureButton&) = delete;
    TextureButton& operator=(TextureButton&&)      = delete;

    void onMouse() override
    {
        sprite_.setColor(OnMouseColor);
    }

    void onClick() override
    {
        sprite_.setColor(OnClickColor);
    }

    void onRelease() override
    {
        sprite_.setColor(sf::Color::White);
    }
    void outMouse() override
    {
        sprite_.setColor(sf::Color::White);
    }

    void draw() override
    {
        if (!getIsHidden())
        {
            sprite_.setPosition(getCords());

            MainWindow.draw(sprite_);
            MainWindow.draw(getText());
        }
    }
};

class RectangleButton : public AButton
{
    const sf::Color OnMouseColor = sf::Color(240, 240, 240);
    const sf::Color OnClickColor = sf::Color(225, 225, 225);

    sf::Color          color_;
    sf::RectangleShape shape_;

public:
    RectangleButton(sf::Vector2f cords, sf::Vector2u size, sf::Color color = sf::Color::White, const std::string& text = " ") :
            AButton(cords,
                    size,
                    text),
            color_  (color),
            shape_  (sf::RectangleShape(sf::Vector2f(static_cast<float>(size.x), static_cast<float>(size.y))))
    {
        shape_.setFillColor(color);
        shape_.setPosition(cords);

        sf::Text tempText = getText();
        tempText.setPosition(getCords().x,
                             getCords().y);
        setText(tempText);
    }
    virtual ~RectangleButton()                         = default;
    RectangleButton (const RectangleButton&)           = default;
    RectangleButton (RectangleButton&& deleted_button) = default;
    RectangleButton& operator=(const RectangleButton&) = default;
    RectangleButton& operator=(RectangleButton&& deleted_button)
    {
        static_cast<AButton>(*this) = std::move(static_cast<AButton>(deleted_button));
        color_ = std::move(deleted_button.color_);
        shape_ = std::move(deleted_button.shape_);

        return *this;
    }

    void setOutlineColor(sf::Color color)
    {
        shape_.setOutlineColor(color);
    }
    void setOutlineThickness(float thickness)
    {
        shape_.setOutlineThickness(thickness);
    }

    void draw() override
    {
        if (!getIsHidden())
        {
            shape_.setSize(sf::Vector2f(static_cast<float>(getSize().x), static_cast<float>(getSize().y)));
            shape_.setPosition(getCords());

            MainWindow.draw(shape_);
            MainWindow.draw(getText());
        }
    }

    void onMouse() override
    {
        sf::Color newColor = color_;
        newColor *= OnMouseColor;
        shape_.setFillColor(newColor);
    }
    void onClick() override
    {
        sf::Color newColor = color_;
        newColor *= OnClickColor;
        shape_.setFillColor(newColor);
    }
    void onRelease() override
    {
        shape_.setFillColor(color_);
    }
    void outMouse() override
    {
        shape_.setFillColor(color_);
    }
};

class AManager
{
    std::vector<AWindow*> windows_;

public:
    AManager() :
            windows_(std::vector<AWindow*>())
    {}
    virtual ~AManager()
    {
        for (auto & window : windows_)
            window->draw();
    }
    AManager (const AManager&)           = delete;
    AManager (AManager&&)                = delete;
    AManager& operator=(const AManager&) = delete;
    AManager& operator=(AManager&&)      = delete;

    void insert(AWindow* wnd)
    {
        windows_.push_back(wnd);
    }

    const std::vector<AWindow*>& getWindows() const
    {
        return windows_;
    }

    void clear()
    {
        windows_.clear();
    }

    virtual void draw()
    {
        for (auto & window : windows_)
            window->draw();
    }
};

class WindowsManager : public AManager
{

public:
    WindowsManager() :
        AManager()
    {}
    virtual ~ WindowsManager()                       = default;
    WindowsManager (const  WindowsManager&)          = delete;
    WindowsManager ( WindowsManager&&)               = delete;
    WindowsManager& operator=(const WindowsManager&) = delete;
    WindowsManager& operator=(WindowsManager&&)      = delete;
};

class ScrollBar : public AManager, public AWindow
{
    const static unsigned ScrollBarHeight = 20;
    const static unsigned StringsOnScreen = 3;

    const sf::Color ArrowColor  = sf::Color(234, 234, 234);
    const sf::Color FieldColor  = sf::Color(218, 218, 218);
    const sf::Color CenterColor = sf::Color(164, 164, 164);

    unsigned        lengthPixels_;
    unsigned        totalStrings_;
    unsigned        currentString_;

    RectangleButton leftArrow_;
    RectangleButton rightArrow_;
    RectangleButton leftField_;
    RectangleButton rightField_;
    RectangleButton centerField_;

public:
    friend class ScrollBarC;

    ScrollBar(sf::Vector2f cords, unsigned lengthPixels, unsigned totalStrings, unsigned currentString = 0) :
        AWindow(cords,
                sf::Vector2u(lengthPixels, ScrollBarHeight)),
        lengthPixels_  (lengthPixels),
        totalStrings_  (totalStrings),
        currentString_ (0),
        leftArrow_     (cords, sf::Vector2u(ScrollBarHeight, ScrollBarHeight), ArrowColor),
        rightArrow_    (cords, sf::Vector2u(ScrollBarHeight, ScrollBarHeight), ArrowColor),
        leftField_     (cords, sf::Vector2u(ScrollBarHeight, ScrollBarHeight), FieldColor),
        rightField_    (cords, sf::Vector2u(ScrollBarHeight, ScrollBarHeight), FieldColor),
        centerField_   (cords, sf::Vector2u(ScrollBarHeight, ScrollBarHeight), CenterColor)
    {
        ASSERT(totalStrings_ >= currentString_);
        ASSERT(lengthPixels_ >= ScrollBarHeight * (4 + StringsOnScreen));

        setSize(sf::Vector2u(lengthPixels_, ScrollBarHeight));

        resize(totalStrings_);
        move(static_cast<int>(currentString));

        assert(currentString_ < totalStrings_);
    }
    virtual ~ScrollBar()                   = default;
    ScrollBar (const ScrollBar&)           = delete;
    ScrollBar (ScrollBar&&)                = delete;
    ScrollBar& operator=(const ScrollBar&) = delete;
    ScrollBar& operator=(ScrollBar&&)      = delete;

    bool resize(unsigned newTotalStrings)
    {
        totalStrings_  = newTotalStrings;

        double f_ScrollBarHeight = static_cast<double>(ScrollBarHeight);
        double f_lengthPixels_   = static_cast<double>(lengthPixels_);
        double f_totalStrings_   = static_cast<double>(totalStrings_);
        double f_currentString_  = static_cast<double>(currentString_);

        double fieldLength = (f_lengthPixels_ - f_ScrollBarHeight * 2) / f_totalStrings_;

        double fcenterField_Size = fieldLength * StringsOnScreen;
        // The hardest part of move. We should consider two possibilities:
        double fleftField_Size   = totalStrings_ >= StringsOnScreen + currentString_

                                    // 1) centerField has normal position and currentString not so big
                                    ? fieldLength * f_currentString_

                                    // 2) centerField on the left and currentString is near to the totalStrings
                                    : f_lengthPixels_ - f_ScrollBarHeight * 2 - fcenterField_Size;

        // The others just combination of leftField_Size
        double frightField_Size  = f_lengthPixels_ - fleftField_Size - fcenterField_Size - f_ScrollBarHeight * 2;

        //printf("fieldLength: %u\nleft: %u\ncenter: %u\nright: %u\n", fieldLength, leftField_Size, centerField_Size, rightField_Size);

        unsigned leftField_Size   = static_cast<unsigned>(fleftField_Size);
        unsigned rightField_Size  = static_cast<unsigned>(frightField_Size);
        unsigned centerField_Size = static_cast<unsigned>(fcenterField_Size);

        leftArrow_.setSize   (sf::Vector2u(ScrollBarHeight,  ScrollBarHeight));
        leftField_.setSize   (sf::Vector2u(leftField_Size,   ScrollBarHeight));
        centerField_.setSize (sf::Vector2u(centerField_Size, ScrollBarHeight));
        rightField_.setSize  (sf::Vector2u(rightField_Size,  ScrollBarHeight));
        rightArrow_.setSize  (sf::Vector2u(ScrollBarHeight,  ScrollBarHeight));

        return true;
    }

    bool move(int offset)
    {
        if (offset >= 0)
        {
            currentString_ += static_cast<unsigned>(offset);
            if (currentString_ > totalStrings_)
                currentString_ = totalStrings_;
        }
        else
        {
            unsigned uoffset = static_cast<unsigned>(offset * (-1));
            if (uoffset > currentString_)
                currentString_ = 0;
            else
                currentString_ = currentString_ - uoffset;
        }
        resize(totalStrings_);

        //printf("New string: %u\n", currentString_);

        float leftField_X   = static_cast<float>(ScrollBarHeight);
        float centerField_X = leftField_X   + static_cast<float>(leftField_.getSize().x);
        float rightField_X  = centerField_X + static_cast<float>(centerField_.getSize().x);
        float rightArrow_X  = rightField_X  + static_cast<float>(rightField_.getSize().x);

        //printf("cords: %f\nleft_X: %f\ncenter_X: %f\nright_X: %f\n", getCords().x, leftField_X, centerField_X, rightField_X);

        leftArrow_.setCords   (getCords());
        leftField_.setCords   (sf::Vector2f(getCords().x + leftField_X,   getCords().y));
        centerField_.setCords (sf::Vector2f(getCords().x + centerField_X, getCords().y));
        rightField_.setCords  (sf::Vector2f(getCords().x + rightField_X,  getCords().y));
        rightArrow_.setCords  (sf::Vector2f(getCords().x + rightArrow_X,  getCords().y));

        assert(currentString_ < totalStrings_);
        return true;
    }

// TODO complete function
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-variable"
    bool moveCenter (int i_offsetFromCenter, int newCenterX)
    {
        float offsetFromLeft   = static_cast<float>(newCenterX) - leftField_.getCords().x;
        float offsetFromCenter = static_cast<float>(i_offsetFromCenter);
        float fieldsLength     = static_cast<float>(lengthPixels_) - static_cast<float>(2 * ScrollBarHeight);
        float f_totalStrings   = static_cast<float>(totalStrings_);
        float CenterSize       = static_cast<float>(centerField_.getSize().x);

        float offset = (offsetFromLeft) / fieldsLength * f_totalStrings;
        //printf("Left:   %f\nRight:  %f\nfields: %f\noffset: %f\n\n", offsetFromLeft, offsetFromCenter, fieldsLength, offset);
        return move(static_cast<int>(offset) - static_cast<int>(currentString_));
    }
#pragma GCC diagnostic pop

    int getOffsetFromCenter(int x) const noexcept
    {
        return x - static_cast<int>(centerField_.getCords().x);
    }

    void draw() override
    {
        if (!getIsHidden())
        {
            leftArrow_.draw();
            rightArrow_.draw();
            leftField_.draw();
            rightField_.draw();
            centerField_.draw();
        }
    }
};

class FileList : public AWindow, public AManager
{
    const unsigned NumberColumns = 3;
    const unsigned NumberRows    = 7;
    const unsigned FilesOnPage   = NumberColumns * NumberRows;

    std::vector<RectangleButton*> buttons_;
    std::vector<std::string>      files_;
    unsigned                      currentPage_;
    unsigned                      totalPages_;

public:
    friend class FileListC;

    FileList(sf::Vector2f cords, sf::Vector2u size, std::vector<std::string> files) :
        AWindow      (cords, size),
        buttons_     (),
        files_       (files),
        currentPage_ (0),
        totalPages_  (static_cast<unsigned>(files.size()) / (FilesOnPage))
    {
        unsigned buttonSizeX = size.x / NumberColumns;
        unsigned buttonSizeY = size.y / NumberRows;
        sf::Vector2u buttonSize(buttonSizeX, buttonSizeY);

        for (unsigned i = 0; i < NumberColumns; ++i)
        {
            for (unsigned j = 0; j < NumberRows; ++j)
            {
                sf::Vector2f curCoords(static_cast<float>(i * buttonSizeX) + cords.x,
                                       static_cast<float>(j * buttonSizeY) + cords.y);
                RectangleButton* rbutton = new RectangleButton(curCoords, buttonSize);
                rbutton->setOutlineThickness(1);
                rbutton->setOutlineColor(sf::Color::Black);

                buttons_.push_back(rbutton);
                dynamic_cast<AManager*>(this)->insert(dynamic_cast<AWindow*>(rbutton));
            }
        }
        updateFiles(100);
    }

    virtual ~FileList()
    {
        for (unsigned long long i = 0; i < buttons_.size(); ++i)
            delete buttons_[i];
    }
    FileList (const FileList&)           = delete;
    FileList (FileList&&)                = delete;
    FileList& operator=(const FileList&) = delete;
    FileList& operator=(FileList&&)      = delete;

    bool updateFiles(int offset)
    {
        unsigned oldCurrentPage = currentPage_;
        // offset -> currentPage
        if (offset >= 0)
        {
            currentPage_ += static_cast<unsigned>(offset);
            if (currentPage_ > totalPages_)
                currentPage_ = totalPages_;
        }
        else
        {
            unsigned uoffset = static_cast<unsigned>(offset * (-1));
            if (uoffset > currentPage_)
                currentPage_ = 0;
            else
                currentPage_ = currentPage_ - uoffset;
        }
        if (oldCurrentPage == currentPage_)
            return true;

        for (unsigned long long i = 0; i < buttons_.size() && (i + currentPage_ * FilesOnPage) < files_.size(); ++i)
        {
            // update file_names
            std::string newStr = files_[i + currentPage_ * FilesOnPage];
            buttons_[i]->setString(newStr);
            sf::Text curText = buttons_[i]->getText();
            float maxStrSize = static_cast<float>(this->getSize().x / NumberColumns);

            if (curText.getLocalBounds().width > maxStrSize)
            {
                float letterSize  = curText.getLocalBounds().width / static_cast<float>(newStr.size());
                float outgoingSize = (maxStrSize - curText.getLocalBounds().width) / letterSize;

                unsigned numberDeleteLetters = 1 + static_cast<unsigned>(outgoingSize / letterSize);

                newStr = newStr.substr(0, newStr.size() - numberDeleteLetters);
                newStr[newStr.size() - 1] = '.';
                newStr[newStr.size() - 2] = '.';
                buttons_[i]->setString(newStr);
            }
        }

        return 1;
    }

    void draw() override
    {
        if (!getIsHidden())
        {
            for (unsigned long long i = 0; i < buttons_.size(); ++i)
                buttons_[i]->draw();
        }
    }
};

//
// C - means controller
//
class AWindowC
{
    AWindow* window_;

public:
    AWindowC(AWindow* window) :
        window_(window)
    {}
    virtual ~AWindowC()                  = default;
    AWindowC (const AWindowC&)           = delete;
    AWindowC (AWindowC&& deleted_window) = delete;
    AWindowC& operator=(const AWindowC&) = delete;
    AWindowC& operator=(AWindowC&&)      = delete;

    virtual bool testMouse(int int_mouseCordsX, int int_mouseCordsY)
    {
        float mouseCordsX = static_cast<float>(int_mouseCordsX);
        float mouseCordsY = static_cast<float>(int_mouseCordsY);

        if (mouseCordsX >=                    window_->getCords().x &&
            mouseCordsY >=                    window_->getCords().y &&
            mouseCordsX <= static_cast<float>(window_->getSize().x) + window_->getCords().x &&
            mouseCordsY <= static_cast<float>(window_->getSize().y) + window_->getCords().y)
            return true;
        else
            return false;
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
    virtual void onMouse(int x, int y)
    {
        window_->onMouse();
    }
    virtual void onClick(int x, int y, sf::Mouse::Button button)
    {
        window_->onClick();
    }
    virtual void onRelease(int x, int y, sf::Mouse::Button button)
    {
        window_->onRelease();
    }
    virtual void outMouse(int x, int y)
    {
        window_->outMouse();
    }
#pragma GCC diagnostic pop

    void draw()
    {
        window_->draw();
    }
};

class AManagerC
{
    AManager* aManager_;

    std::vector<AWindowC*>        windowCs_;
    std::unordered_set<AWindowC*> focusedWindows_;
    std::unordered_set<AWindowC*> clickedWindows_;
public:
    AManagerC(AManager* aManager) :
        aManager_       (aManager),
        windowCs_       (std::vector<AWindowC*>(0)),
        focusedWindows_ (std::unordered_set<AWindowC*>(0)),
        clickedWindows_ (std::unordered_set<AWindowC*>(0))
    {}
    virtual ~AManagerC()                   = default;
    AManagerC (const AManagerC&)           = delete;
    AManagerC (AManagerC&&)                = delete;
    AManagerC& operator=(const AManagerC&) = delete;
    AManagerC& operator=(AManagerC&&)      = delete;

    void insert(AWindowC* wnd)
    {
        windowCs_.push_back(wnd);
    }

    void onEventManager(sf::Event event)
    {
        if (event.type == sf::Event::MouseMoved)
            onMouseManager(event);
        if (event.type == sf::Event::MouseButtonPressed)
            onClickManager(event);
        if (event.type == sf::Event::MouseButtonReleased)
            onReleaseManager(event);
    }

    void onMouseManager(sf::Event event)
    {
        std::unordered_set<AWindowC*> unfocusedWindows = focusedWindows_;

        for (unsigned long long i = 0; i < windowCs_.size(); ++i)
            if (windowCs_[i]->testMouse(event.mouseMove.x, event.mouseMove.y))
            {
                // avoid clicked windows
                if (clickedWindows_.find(windowCs_[i]) != clickedWindows_.end())
                    continue;

                // if already focused, avoid it
                if (focusedWindows_.find(windowCs_[i]) != focusedWindows_.end())
                {
                    unfocusedWindows.erase(windowCs_[i]);
                    continue;
                }

                focusedWindows_.insert(windowCs_[i]);
                windowCs_[i]->onMouse(event.mouseMove.x, event.mouseMove.y);
            }

        // unfocus the remaining windows
        for (auto itr = unfocusedWindows.begin(); itr != unfocusedWindows.end(); ++itr)
        {
            assert(focusedWindows_.find(*itr) != focusedWindows_.end());

            focusedWindows_.erase(*itr);
            (*itr)->outMouse(event.mouseMove.x, event.mouseMove.y);
        }
    }
    void onClickManager(sf::Event event)
    {
        assert(clickedWindows_.size() == 0);

        for (unsigned long long i = 0; i < windowCs_.size(); ++i)
            if (windowCs_[i]->testMouse(event.mouseButton.x, event.mouseButton.y))
            {
                clickedWindows_.insert(windowCs_[i]);

                // clicked windows MUST NOT be in the focus windows
                if (focusedWindows_.find(windowCs_[i]) != focusedWindows_.end())
                    focusedWindows_.erase(windowCs_[i]);

                windowCs_[i]->onClick(event.mouseButton.x, event.mouseButton.y, event.mouseButton.button);
            }
    }
    void onReleaseManager(sf::Event event)
    {
        for (auto itr = clickedWindows_.begin(); itr != clickedWindows_.end(); ++itr)
            (*itr)->onRelease(event.mouseButton.x, event.mouseButton.y, event.mouseButton.button);
        clickedWindows_.clear();
    }

    void drawManager()
    {
        aManager_->draw();
    }
};

class ButtonC : public AWindowC
{
    AButton* button_;

public:
    ButtonC(AButton* button) :
        AWindowC(dynamic_cast<AWindow*>(button)),
        button_(button)
    {}
    virtual ~ButtonC()                 = default;
    ButtonC (const ButtonC&)           = delete;
    ButtonC (ButtonC&&)                = delete;
    ButtonC& operator=(const ButtonC&) = delete;
    ButtonC& operator=(ButtonC&&)      = delete;

    bool testClick() const
    {
        return button_->getIsFocus();
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
    void onClick(int x, int y, sf::Mouse::Button button) override
    {
        button_->setIsFocus(true);
        button_->onClick();
    }
    void onRelease(int x, int y, sf::Mouse::Button button) override
    {
        button_->setIsFocus(false);
        button_->onRelease();
    }
#pragma GCC diagnostic pop
};

class WindowsManagerC : public AManagerC
{

public:
    WindowsManagerC(AManager* aManager) :
            AManagerC(aManager)
    {}
    virtual ~WindowsManagerC()                         = default;
    WindowsManagerC (const WindowsManagerC&)           = delete;
    WindowsManagerC (WindowsManagerC&&)                = delete;
    WindowsManagerC& operator=(const WindowsManagerC&) = delete;
    WindowsManagerC& operator=(WindowsManagerC&&)      = delete;
};

class ScrollBarC : public AWindowC, public AManagerC
{
    ScrollBar* scrollBar_;
    //FileListC fil

    ButtonC leftArrowC_;
    ButtonC rightArrowC_;
    ButtonC leftFieldC_;
    ButtonC rightFieldC_;
    ButtonC centerFieldC_;

    bool isCenterClicked_;
    int  centerClickX_;

public:
    ScrollBarC(ScrollBar* scrollBar) :
        AWindowC         (dynamic_cast<AWindow*>(scrollBar)),
        AManagerC        (dynamic_cast<AManager*>(scrollBar)),
        scrollBar_       (scrollBar),
        leftArrowC_      (&(scrollBar_->leftArrow_)),
        rightArrowC_     (&(scrollBar_->rightArrow_)),
        leftFieldC_      (&(scrollBar_->leftField_)),
        rightFieldC_     (&(scrollBar_->rightField_)),
        centerFieldC_    (&(scrollBar_->centerField_)),
        isCenterClicked_ (0),
        centerClickX_    (0)
    {
        insert(dynamic_cast<AWindowC*>(&leftArrowC_));
        insert(dynamic_cast<AWindowC*>(&rightArrowC_));
        insert(dynamic_cast<AWindowC*>(&leftFieldC_));
        insert(dynamic_cast<AWindowC*>(&rightFieldC_));
        insert(dynamic_cast<AWindowC*>(&centerFieldC_));
    }
    virtual ~ScrollBarC()                    = default;
    ScrollBarC (const ScrollBarC&)           = delete;
    ScrollBarC (ScrollBarC&&)                = delete;
    ScrollBarC& operator=(const ScrollBarC&) = delete;
    ScrollBarC& operator=(ScrollBarC&&)      = delete;

    bool testMouse(int int_mouseCordsX, int int_mouseCordsY) override
    {
        float mouseCordsX = static_cast<float>(int_mouseCordsX);
        float mouseCordsY = static_cast<float>(int_mouseCordsY);

        if (mouseCordsX >=                    scrollBar_->getCords().x &&
            mouseCordsY >=                    scrollBar_->getCords().y &&
            mouseCordsX <= static_cast<float>(scrollBar_->getSize().x) + scrollBar_->getCords().x &&
            mouseCordsY <= static_cast<float>(scrollBar_->getSize().y) + scrollBar_->getCords().y)
        {
            onMouse(int_mouseCordsX, int_mouseCordsY);
            return true;
        }

        onMouse(int_mouseCordsX, int_mouseCordsY);
        return false;
    }

    void onMouse(int x, int y) override
    {
        sf::Event event;
        event.type = sf::Event::EventType::MouseMoved;
        event.mouseMove.x = x;
        event.mouseMove.y = y;
        onMouseManager(event);

        if (isCenterClicked_)
            scrollBar_->moveCenter(centerClickX_, x);
    }
    void onClick(int x, int y, sf::Mouse::Button button) override
    {
        sf::Event event;
        event.type = sf::Event::EventType::MouseButtonPressed;
        event.mouseButton.x = x;
        event.mouseButton.y = y;
        event.mouseButton.button = button;
        onClickManager(event);

        if (leftArrowC_.testClick())
            scrollBar_->move(-1);

        else if (rightArrowC_.testClick())
            scrollBar_->move(1);

        else if (leftFieldC_.testClick())
            scrollBar_->move(-static_cast<int>(ScrollBar::StringsOnScreen));

        else if (rightFieldC_.testClick())
            scrollBar_->move(static_cast<int>(ScrollBar::StringsOnScreen));

        else if (centerFieldC_.testClick())
        {
            isCenterClicked_ = true;
            centerClickX_ = scrollBar_->getOffsetFromCenter(x);
        }

    }
    void onRelease(int x, int y, sf::Mouse::Button button) override
    {
        sf::Event event;
        event.type = sf::Event::EventType::MouseButtonReleased;
        event.mouseButton.x = x;
        event.mouseButton.y = y;
        event.mouseButton.button = button;
        onReleaseManager(event);

        isCenterClicked_ = false;
    }

};

class FileListC : public AWindowC, public AManagerC
{
    FileList* fileList_;
    std::vector<AWindowC> files_;

public:
    FileListC(FileList* fileList) :
        AWindowC  (static_cast<AWindow*>(fileList)),
        AManagerC (static_cast<AManager*>(fileList)),
        fileList_ (fileList),
        files_    ()
    {
        const std::vector<AWindow*>& aWindows = fileList_->getWindows();
        for (unsigned long long i = 0; i < fileList_->buttons_.size(); ++i)
        {
            files_.push_back(AWindowC(aWindows[i]));
            dynamic_cast<AManagerC*>(this)->insert(&files_[i]);
        }
    }
    virtual ~FileListC()                   = default;
    FileListC (const FileListC&)           = delete;
    FileListC (FileListC&&)                = delete;
    FileListC& operator=(const FileListC&) = delete;
    FileListC& operator=(FileListC&&)      = delete;

    bool testMouse(int int_mouseCordsX, int int_mouseCordsY) override
    {
        float mouseCordsX = static_cast<float>(int_mouseCordsX);
        float mouseCordsY = static_cast<float>(int_mouseCordsY);

        if (mouseCordsX >=                    fileList_->getCords().x &&
            mouseCordsY >=                    fileList_->getCords().y &&
            mouseCordsX <= static_cast<float>(fileList_->getSize().x) + fileList_->getCords().x &&
            mouseCordsY <= static_cast<float>(fileList_->getSize().y) + fileList_->getCords().y)
        {
            onMouse(int_mouseCordsX, int_mouseCordsY);
            return true;
        }

        onMouse(int_mouseCordsX, int_mouseCordsY);
        return false;
    }

    void onMouse(int x, int y) override
    {
        sf::Event event;
        event.type = sf::Event::EventType::MouseMoved;
        event.mouseMove.x = x;
        event.mouseMove.y = y;
        onMouseManager(event);
    }
    void onClick(int x, int y, sf::Mouse::Button button) override
    {
        sf::Event event;
        event.type = sf::Event::EventType::MouseButtonPressed;
        event.mouseButton.x = x;
        event.mouseButton.y = y;
        event.mouseButton.button = button;
        onClickManager(event);

        /*
        if (leftArrowC_.testClick())
            scrollBar_->move(-1);

        else if (rightArrowC_.testClick())
            scrollBar_->move(1);

        else if (leftFieldC_.testClick())
            scrollBar_->move(-static_cast<int>(ScrollBar::StringsOnScreen));

        else if (rightFieldC_.testClick())
            scrollBar_->move(static_cast<int>(ScrollBar::StringsOnScreen));

        else if (centerFieldC_.testClick())
        {
            isCenterClicked_ = true;
            centerClickX_ = scrollBar_->getOffsetFromCenter(x);
        }
        */
    }
    void onRelease(int x, int y, sf::Mouse::Button button) override
    {
        sf::Event event;
        event.type = sf::Event::EventType::MouseButtonReleased;
        event.mouseButton.x = x;
        event.mouseButton.y = y;
        event.mouseButton.button = button;
        onReleaseManager(event);
    }

};

int main()
{
    std::string path = "C:\\Windows";
    std::vector<std::string> files{};
    for (const auto & entry : std::experimental::filesystem::directory_iterator(path))
    {
        std::string temp_string = entry.path().string().substr(path.size() + 1, std::string::npos);
        files.push_back(temp_string);
        //std::cout << temp_string << std::endl;
    }

    TextureButton gobtn       (GreenButton, sf::Vector2f(800, 150), "GO");
    ButtonC       gobtn_contr (&gobtn);

    ScrollBar     scrlb       (sf::Vector2f(2, 300), 600, 20, 6);
    ScrollBarC    scrlb_contr (&scrlb);

    FileList      flist       (sf::Vector2f(100, 100), sf::Vector2u(600, 200), files);

    WindowsManager wmanager;
    wmanager.insert(&gobtn);
    wmanager.insert(&scrlb);
    wmanager.insert(&flist);

    WindowsManagerC wmanager_contr(&wmanager);
    wmanager_contr.insert(&gobtn_contr);
    wmanager_contr.insert(&scrlb_contr);

    while (MainWindow.isOpen())
    {
        sf::Event event{};
        while (MainWindow.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                MainWindow.close();
            wmanager_contr.onEventManager(event);
        }

        MainWindow.clear(sf::Color::White);
        wmanager.draw();
        MainWindow.display();
    }
    return 0;
}