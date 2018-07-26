# Source files
set(SOURCES
    OGSError.cpp
    QNonScalableGraphicsTextItem.cpp
    RecentFiles.cpp
    TreeItem.cpp
    TreeModel.cpp
    ColorPickerPushButton.cpp
    TreeModelIterator.cpp
    CheckboxDelegate.cpp
    QValueTooltipSlider.cpp
)

# Header files
set(HEADERS
    LastSavedFileDirectory.h
    OGSError.h
    QNonScalableGraphicsTextItem.h
    TreeItem.h
    TreeModelIterator.h
    StrictDoubleValidator.h
    RecentFiles.h
    TreeModel.h
    ColorPickerPushButton.h
    CheckboxDelegate.h
    QValueTooltipSlider.h
)

source_group("UI Files" REGULAR_EXPRESSION "\\w*\\.ui")

# Create the library
add_library(QtBase
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(QtBase Qt5::Widgets)

set_property(TARGET QtBase PROPERTY FOLDER "DataExplorer")

if(OGS_USE_PCH)
    cotire(QtBase)
endif()
