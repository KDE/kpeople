set (git_log_viewer_SRCS
    main.cpp
    dialog.cpp
    logmodel.cpp
)

kde4_add_ui_files(git_log_viewer_SRCS dialog.ui)
kde4_add_executable(git-log-viewer ${git_log_viewer_SRCS})

target_link_libraries(git-log-viewer
                       ${QT_QTCORE_LIBRARY}
                       ${QT_QTGUI_LIBRARY}
                       ${KDE4_KDEUI_LIBS}
                       kpeople)
