
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
            fm.text("Depaver " + fname);
            grouper.readfileAdjancylist(fname);
            grouper.assign();
            grouper.writeGroupList();
            grouper.writeAssignTable();
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
            grouper.writeGroupList();
            grouper.writeAssignTable();
            fm.update();
        });

    /////////////////// Edit //////////////

    wex::menu edit(fm);
    edit.append(
        "Algorithm",
        [&](const std::string &title)
        {
            auto ap = grouper.algoParams();
            wex::inputbox ib;
            ib.labelWidth(200);
            ib.gridWidth(300);
            ib.text("Edit grouper algorithm parameters");
            ib.add("Minimum group sum", std::to_string(ap.MinSum));
            ib.add("Maximum group sum", std::to_string(ap.MaxSum));
            ib.add("Minimum group size", std::to_string(ap.MinSize));
            ib.showModal();
            grouper.algoParams(
                atof(ib.value("Minimum group sum").c_str()),
                atof(ib.value("Maximum group sum").c_str()),
                atoi(ib.value("Minimum group size").c_str()));
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

    ///////////// View /////////////

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

    //////////////////// Help /////////////////////////

    wex::menu help(fm);
    help.append(
        "About",
        [&](const std::string &title)
        {
            wex::msgbox mb(
                fm,
                "Depaver identifies municipalities that can share land takes.\n\n"
                "Land take is the conversion of natural land to sealed soil as a result of urbanization.\n\n"
                "European policy aims to halt land take by reducing it by 50% by 2030.\n"
                "The input data is a series of surplus/deficits for each locality measured in hectares based on its needs for the next decade. "
                "Surplus means the locality has extra hectares compared to the 50%, deficits means they have less. "
                "Localities can share hectares.\n\n"
                "(c) 2023 James Bremner",
                "About the Depaver Application",
                (unsigned int)0);
        });
    mbar.append("Help", help);
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
