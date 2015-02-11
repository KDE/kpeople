# KPeople

Provides access to all contacts and aggregates them by person.

## Introduction

KPeople offers unified access to our contacts from different sources, grouping
them by person while still exposing all the data.

Furthermore, KPeople will also provide facilities to integrate the data provided
in user interfaces by providing QML and Qt Widgets components.

The sources are plugin-based, allowing to easily extend the contacts collection.

## Usage

To use KPeople, you'll want to look for it using

    find_package(KF5People)

Or its qmake counterpart. Then you'll have available 3 different libraries to
depend on:
* KPeople which is the most basic one and will mainly provide PersonsModel
and PersonData. You will use them respectively in case you need information
about the full set of contacts on your system or just the one.

* KPeopleWidgets will be required in case we want to integrate it into your
QtWidgets-based application. We will mainly find the PersonDetailsView and
the MergeDialog.

* KPeopleBackend is the library used to make it possible to create backends
for KPeople. You should _only_ use it in case you are providing support for
a new backend type. *WARNING* Its ABI interfaces are not yet stable.

* Also KPeople also bundles a qml import module that can be pulled using:

    import org.kde.people 1.0

This will allow access to the KPeople data from QML code-bases.

