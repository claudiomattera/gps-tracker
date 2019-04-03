import QtQuick 2.0
import QtPositioning 5.5
import QtLocation 5.6

Rectangle {
    anchors.fill: parent

    Plugin {
        id: osmMapPlugin
        name: "osm"
    }

    Map {
        id: map
        anchors.fill: parent
        plugin: osmMapPlugin
        zoomLevel: 14
        center: pathController.center

        MapPolyline {
            id: pl
            line.width: 3
            line.color: 'red'
        }
    }

    function loadPath(){
        var lines = []
        for(var i = 0; i < pathController.geopath.size(); i++){
            lines[i] = pathController.geopath.coordinateAt(i)
        }
        return lines;
    }
    Connections{
        target: pathController
        onGeopathChanged: {
            pl.path = loadPath();
        }
        onCenterChanged: {
            map.center = pathController.center;
        }
    }

    Component.onCompleted: pl.path = loadPath()
}
