
#include <filesystem>

#include "cGUI.h"

void cGUI::constructMenu()
{
    wex::menubar mbar(fm);

    wex::menu file(fm);
    file.append(
        "Adjacency List",
        [&](const std::string &title)
        {
            wex::filebox fb(fm);
            auto fname = fb.open();
            fm.text("Grouper " + fname);
            muni.readfile2(fname);
            muni.assign();
            muni.display();
            fm.update();
        });
    mbar.append("File", file);

    wex::menu view(fm);
    // view.append(
    //     "Input",
    //     [&](const std::string &title)
    //     {
    //         // std::ifstream ifs(thePirateIslands.myfname);
    //         // std::stringstream buffer;
    //         // buffer << ifs.rdbuf();
    //         // results = buffer.str();
    //         fm.update();
    //     });
    view.append(
        "Groups, text",
        [&](const std::string &title)
        {
            myDisplay = eDisplay::text;
            fm.update();
        });
    view.append(
        "Groups, layout",
        [this](const std::string &title)
        {
            myDisplay = eDisplay::layout;
            fm.update();
        });
    mbar.append("View", view);
}
void cGUI::draw()
{
    switch (myDisplay)
    {
    case eDisplay::text:
        pldisplay.show();
        pldisplay.text(muni.text());
        pldisplay.update();
        break;

    case eDisplay::layout:
        pldisplay.show(false);
        wex::window2file w2f;
        auto path = std::filesystem::temp_directory_path();
        auto sample = path / "sample.png";
        w2f.draw(fm, sample.string());
        break;
    }
}
