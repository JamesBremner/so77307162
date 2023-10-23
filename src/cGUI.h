#include <string>

#include <wex.h> // https://github.com/JamesBremner/windex
#include <window2file.h>

#include "cGrouper.h"

class cStarterGUI
{
public:
    /** CTOR
     * @param[in] title will appear in application window title
     * @param[in] vlocation set location and size of appplication window
     * 
     * Usage:
     * 
     * <pre>
class appGUI : public cStarterGUI
{
public:
    appGUI()
        : cStarterGUI(
              "The Appliccation",
              {50, 50, 1000, 500})
    {

        // initialize the solution
        ...

        show();
        run();
    }
    </pre>
    */
    cStarterGUI(
        const std::string &title,
        const std::vector<int> &vlocation)
        : fm(wex::maker::make())
    {
        fm.move(vlocation);
        fm.text(title);

        fm.events().draw(
            [&](PAINTSTRUCT &ps)
            {
                wex::shapes S(ps);
                draw(S);
            });
    }
    /** Draw nothing
     * 
     * An application should over-ride this method
     * to perform any drawing reuired
     */
    virtual void draw( wex::shapes& S )
    {

    }
    void show()
    {
        fm.show();
    }
    void run()
    {
        fm.run();
    }

protected:
    wex::gui &fm;
};


class cGUI : public cStarterGUI
{
public:
    cGUI()
        : cStarterGUI(
              "Grouper",
              {50, 50, 1000, 500}),
        pldisplay(wex::maker::make<wex::panel>(fm))
    {
        constructMenu();

        pldisplay.move(0,0,1000,500);
        myDisplay = eDisplay::text;

        fm.events().draw(
            [this](PAINTSTRUCT &ps)
            {
                draw();
            });

        show();
        run();
    }

private:
    cGrouper muni;

    enum class eDisplay
    {
        input,
        text,
        layout,
    };

    eDisplay myDisplay;

    wex::panel& pldisplay;

    void constructMenu();
    void draw();
};
