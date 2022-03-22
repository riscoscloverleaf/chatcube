# this one is important
SET(CMAKE_SYSTEM_NAME GNU)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(riscosgcc /home/lenz/gccsdk)
set(CMAKE_C_COMPILER ${riscosgcc}/env/arm-unknown-riscos-gcc)
set(CMAKE_CXX_COMPILER ${riscosgcc}/env/arm-unknown-riscos-g++)
set(CMAKE_AR ${riscosgcc}/cross/bin/arm-unknown-riscos-ar)

add_library(tbx
        abouttobeshownlistener.cc
        actionbutton.cc
        adjuster.cc
        application.cc
        button.cc
        caret.cc
        clipboard.cc
        colourdbox.cc
        colourmenu.cc
        component.cc
        commandrouter.cc
        dcs.cc
        displayfield.cc
        draggable.cc
        drawfile.cc
        drawpath.cc
        eventrouter.cc
        fileinfo.cc
        fileraction.cc
        font.cc
        fontdbox.cc
        fontmenu.cc
        gadget.cc
        hasbeenhiddenlistener.cc
        hourglass.cc
        iconbar.cc
        iconpainter.cc
        jpeg.cc
        loader.cc
        loadermanager.cc
        lockextentposition.cc
        memoryloader.cc
        menu.cc
        message.cc
        messagefile.cc
        messagewindow.cc
        numberrange.cc
        object.cc
        objectdeletedlistener.cc
        openermanager.cc
        optionbutton.cc
        osgraphics.cc
        path.cc
        pointerinfo.cc
        popup.cc
        prequitlistener.cc
        printdbox.cc
        proginfo.cc
        propertyset.cc
        pthreadguard.cc
        questionwindow.cc
        quit.cc
        radiobutton.cc
        reporterror.cc
        saveas.cc
        saver.cc
        scale.cc
        scrolllist.cc
        showfullobject.cc
        showhelp.cc
        showpointobject.cc
        slider.cc
        sprite.cc
        stringset.cc
        tag.cc
        taskmanager.cc
        taskwindow.cc
        textarea.cc
        textdisplaywindow.cc
        toolaction.cc
        uri.cc
        usereventlistener.cc
        wimpmessagelistener.cc
        window.cc
        writablefield.cc
        doc/docfileinfo.cc
        doc/dociconbar.cc
        doc/docmanager.cc
        doc/docpositionwindow.cc
        doc/docsaveas.cc
        doc/document.cc
        doc/docwindow.cc
        ext/oleclient.cc
        ext/stronghelp.cc
        ext/tabs.cc
        ext/treeview.cc
        res/resbase.cc
        res/reseditor.cc
        res/resfile.cc
        res/resmenu.cc
        res/resobject.cc
        res/reswindow.cc
        view/fontitemrenderer.cc
        view/gadgetlistview.cc
        view/iconitemrenderer.cc
        view/itemrenderer.cc
        view/itemview.cc
        view/listview.cc
        view/reportview.cc
        view/selection.cc
        view/textview.cc
        view/tileview.cc
        )