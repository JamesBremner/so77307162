
#include <filesystem>

#include "cGUI.h"

#include <inputbox.h>

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
            grouper.readfileAdjancylist(fname);
            grouper.assign();
            grouper.display();
            fm.update();
        });
    mbar.append("File", file);
    file.append(
        "Regions to group",
        [&](const std::string &title)
        {
            wex::inputbox ib;
            ib.labelWidth(200);
            ib.gridWidth(500);
            ib.add("Regions ( Space separated list e.g. \"84 32\")",
                   grouper.regionsIncluded());
            ib.showModal();
            grouper.regionsIncluded(ib.value("Regions ( Space separated list e.g. \"84 32\")"));
            wex::filebox fb(fm);
            auto fname = fb.open();
            fm.text("Grouper " + fname);
            grouper.readfileAdjancylist(fname);
            grouper.assign();
            grouper.display();
            fm.update();
        });

    /////////////////// Edit //////////////

    wex::menu edit(fm);
    edit.append(
        "Algorithm",
        [&](const std::string &title)
        {
            wex::inputbox ib;
            ib.labelWidth(200);
            ib.gridWidth(300);
            ib.text("Edit grouper algorithm parameters");
            ib.add("Minimum group sum", std::to_string(grouper.minSum()));
            ib.add("Maximum group sum", std::to_string(grouper.maxSum()));
            ib.add("Minimum group size", std::to_string(grouper.minSize()));
            ib.showModal();
            grouper.minSum(atof(ib.value("Minimum group sum").c_str()));
            grouper.maxSum(atof(ib.value("Maximum group sum").c_str()));
            grouper.minSize(atof(ib.value("Minimum group size").c_str()));
            try
            {
                grouper.sanity();
            }
            catch (std::exception &e)
            {
                wex::msgbox(
                    std::string("Exception: ") + e.what());
                    return;
            }
        });
    mbar.append("Edit", edit);

    wex::menu view(fm);
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
    view.append(
        "Region",
        [this](const std::string &title)
        {
            wex::inputbox ib;
            ib.labelWidth(200);
            ib.gridWidth(300);
            ib.add("View groups in region",
                   std::to_string(myRegionView));
            ib.showModal();
            myRegionView = atoi(ib.value("View groups in region").c_str());
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
        pldisplay.text(grouper.text(myRegionView));
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
