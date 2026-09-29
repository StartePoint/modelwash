import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import QtQuick3D
import QtQuick3D.Helpers

ApplicationWindow {
    id: root
    width: 1320
    height: 860
    visible: true
    title: qsTr("洗模型 — ModelWash")
    color: "#F2F3F7"

    // WorkBuddy-like soft workstation palette
    property color bg: "#F2F3F7"
    property color surface: "#FFFFFF"
    property color surfaceSoft: "#F7F8FC"
    property color ink: "#1C1F2A"
    property color muted: "#8A90A2"
    property color line: "#E6E8F0"
    property color accent: "#4C6FFF"
    property color accentSoft: "#EEF1FF"
    property color ok: "#2F9B6A"
    property color warn: "#C48A1A"
    property color danger: "#D4534A"
    property int rCard: 16
    property int rCtl: 12

    FileDialog {
        id: openDialog
        title: qsTr("打开模型")
        fileMode: FileDialog.OpenFile
        nameFilters: [ qsTr("3D 模型 (*.glb *.gltf *.obj)"), qsTr("所有文件 (*)") ]
        onAccepted: Session.openFile(selectedFile)
    }
    FileDialog {
        id: saveDialog
        title: qsTr("导出 GLB")
        fileMode: FileDialog.SaveFile
        nameFilters: [ qsTr("GLB (*.glb)") ]
        defaultSuffix: "glb"
        onAccepted: Session.exportGlb(selectedFile)
    }
    Connections {
        target: Session
        function onErrorOccurred(messageZh) { toast.show(messageZh, true) }
        function onInfoOccurred(messageZh) { toast.show(messageZh, false) }
    }

    // ===== Onboarding (hero workstation) =====
    Rectangle {
        anchors.fill: parent
        visible: Session.needsOnboarding
        z: 30
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#EEF1FF" }
            GradientStop { position: 0.45; color: "#F2F3F7" }
            GradientStop { position: 1.0; color: "#F7F8FC" }
        }

        Column {
            anchors.centerIn: parent 
            spacing: 28
            width: 620

            Column {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 8
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "ModelWash"
                    color: root.accent
                    font.pixelSize: 13
                    font.weight: Font.DemiBold
                }
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("先选一个工位身份")
                    color: root.ink
                    font.pixelSize: 30
                    font.weight: Font.DemiBold
                }
                Text {
                    width: 480
                    anchors.horizontalCenter: parent.horizontalCenter
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                    text: qsTr("选一次即可，之后可在设置更改。只影响默认推荐，不锁功能。")
                    color: root.muted
                    font.pixelSize: 14
                }
            }

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 18
                PersonaCard {
                    title: qsTr("我要交付资源")
                    subtitle: qsTr("网页 / 小游戏用，优先体积与兼容。")
                    tag: "AssetShipper"
                    onClicked: Session.completeOnboarding("assetShipper")
                }
                PersonaCard {
                    title: qsTr("我要修 AI 出的模")
                    subtitle: qsTr("先抹平再瘦身，再一键清洗导出。")
                    tag: "AICleanup"
                    onClicked: Session.completeOnboarding("aiCleanup")
                }
            }
        }
    }

    // ===== Main shell =====
    RowLayout {
        anchors.fill: parent
        anchors.margins: 14
        spacing: 12
        visible: !Session.needsOnboarding

        // Narrow icon rail (WorkBuddy-like)
        Rectangle {
            Layout.preferredWidth: 72
            Layout.fillHeight: true
            radius: root.rCard
            color: root.surface
            border.color: root.line

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 10

                Rectangle {
                    Layout.alignment: Qt.AlignHCenter
                    width: 44; height: 44; radius: 12
                    color: "transparent"
                    clip: true
                    Image {
                        anchors.fill: parent
                        source: "qrc:/icons/modelwash.png"
                        fillMode: Image.PreserveAspectFit
                        smooth: true
                        mipmap: true
                    }
                }

                Item { height: 8 }

                NavIcon {
                    label: qsTr("清洗")
                    active: Session.mode === "wash"
                    enabled: true
                    onClicked: Session.mode = "wash"
                }
                NavIcon {
                    label: qsTr("修一修")
                    active: false
                    enabled: false
                    tip: qsTr("下一版本")
                }
                NavIcon {
                    label: qsTr("拼一拼")
                    active: false
                    enabled: false
                    tip: qsTr("下一版本")
                }
                NavIcon {
                    label: qsTr("摆好看")
                    active: false
                    enabled: false
                    tip: qsTr("下一版本")
                }

                Item { Layout.fillHeight: true }

                NavIcon {
                    label: "?"
                    active: false
                    enabled: true
                    onClicked: aboutPopup.open()
                }
            }
        }

        // Center workspace
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 12

            // Soft top bar
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 56
                radius: root.rCard
                color: root.surface
                border.color: root.line

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 14
                    anchors.rightMargin: 14
                    spacing: 8

                    Text {
                        text: qsTr("洗模型")
                        color: root.ink
                        font.pixelSize: 16
                        font.weight: Font.DemiBold
                    }
                    Text {
                        text: qsTr("本地工位 · 不上传")
                        color: root.muted
                        font.pixelSize: 12
                    }

                    Item { Layout.fillWidth: true }

                    SoftBtn { text: qsTr("打开"); onClicked: openDialog.open() }
                    SoftBtn { text: qsTr("撤销"); enabled: Session.canUndo; onClicked: Session.undo() }
                    SoftBtn { text: qsTr("重做"); enabled: Session.canRedo; onClicked: Session.redo() }
                    SoftBtn {
                        text: qsTr("导出")
                        primary: true
                        onClicked: saveDialog.open()
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 12

                // Objects card
                Rectangle {
                    Layout.preferredWidth: 200
                    Layout.fillHeight: true
                    radius: root.rCard
                    color: root.surface
                    border.color: root.line

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 14
                        spacing: 12

                        Text {
                            text: qsTr("工作区")
                            color: root.muted
                            font.pixelSize: 12
                            font.weight: Font.DemiBold
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 128
                            radius: 14
                            color: root.surfaceSoft
                            border.color: Session.hasDocument ? root.accent : root.line

                            Column {
                                anchors.centerIn: parent
                                spacing: 6
                                Text {
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    text: Session.hasDocument ? qsTr("当前模型") : qsTr("还没有模型")
                                    color: root.ink
                                    font.pixelSize: 14
                                    font.weight: Font.DemiBold
                                }
                                Text {
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    text: Session.hasDocument
                                          ? (Session.triangleCount + qsTr(" 三角"))
                                          : qsTr("打开或拖入文件")
                                    color: root.muted
                                    font.pixelSize: 12
                                }
                            }
                        }

                        SoftBtn {
                            Layout.fillWidth: true
                            text: qsTr("+ 导入模型")
                            primary: true
                            onClicked: openDialog.open()
                        }
                        Item { Layout.fillHeight: true }
                    }
                }

                // Viewport card
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    radius: root.rCard
                    color: root.surface
                    border.color: root.line
                    clip: true

                    View3D {
                        id: view
                        anchors.fill: parent
                        anchors.margins: 2
                        camera: camera

                        environment: SceneEnvironment {
                            clearColor: "#E9ECF5"
                            backgroundMode: SceneEnvironment.Color
                            antialiasingMode: SceneEnvironment.MSAA
                            lightProbe: Texture {
                                source: Session.lightProbeSource
                            }
                            probeExposure: 1.15
                        }

                        // OrbitCameraController requires camera to be a child of origin.
                        Node {
                            id: cameraOrigin
                            eulerRotation: Qt.vector3d(-18, 28, 0)
                            PerspectiveCamera {
                                id: camera
                                z: 4.5
                                clipNear: 0.01
                                clipFar: 10000
                            }
                        }

                        DirectionalLight {
                            eulerRotation.x: -42
                            eulerRotation.y: -28
                            brightness: 1.8
                            ambientColor: "#B0B6C0"
                            castsShadow: false
                        }
                        DirectionalLight {
                            eulerRotation.x: 25
                            eulerRotation.y: 145
                            brightness: 0.9
                            ambientColor: "#404650"
                            castsShadow: false
                        }

                        // Native path disabled — mesh preview is the reliable display path.
                        Repeater3D {
                            model: Session.previewParts
                            delegate: Model {
                                required property var meshGeometry
                                required property url matTextureUrl
                                required property bool matHasTexture
                                required property color matBaseColor
                                required property real matMetallic
                                required property real matRoughness

                                visible: Session.hasDocument && Session.hasPreviewMesh
                                geometry: meshGeometry
                                materials: DefaultMaterial {
                                    diffuseColor: matBaseColor
                                    lighting: DefaultMaterial.FragmentLighting
                                    specularAmount: 0.12
                                    specularRoughness: Math.max(0.2, matRoughness)
                                    cullMode: Material.NoCulling
                                    diffuseMap: matHasTexture ? albedoTex : null
                                }
                                Texture {
                                    id: albedoTex
                                    source: matHasTexture ? matTextureUrl : ""
                                    flipV: true
                                    minFilter: Texture.Linear
                                    magFilter: Texture.Linear
                                    generateMipmaps: true
                                }
                            }
                        }

                        Model {
                            visible: !Session.hasDocument
                            source: "#Cube"
                            scale: Qt.vector3d(0.55, 0.55, 0.55)
                            materials: DefaultMaterial {
                                diffuseColor: "#A8B4C8"
                                lighting: DefaultMaterial.FragmentLighting
                            }
                        }

                        DropArea {
                            anchors.fill: parent
                            onDropped: (drop) => {
                                if (drop.hasUrls) Session.openFile(drop.urls[0])
                            }
                        }

                        OrbitCameraController {
                            anchors.fill: parent
                            origin: cameraOrigin
                            camera: camera
                            panEnabled: true
                            mouseEnabled: true
                        }

                        Connections {
                            target: Session
                            function onPreviewChanged() {
                                cameraOrigin.eulerRotation = Qt.vector3d(-18, 28, 0)
                                camera.z = 4.5
                                cameraOrigin.position = Qt.vector3d(0, 0, 0)
                            }
                        }
                    }

                    Rectangle {
                        anchors.bottom: parent.bottom
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.bottomMargin: 14
                        width: hint.implicitWidth + 20
                        height: 28
                        radius: 14
                        color: "#CCFFFFFF"
                        border.color: root.line
                        Text {
                            id: hint
                            anchors.centerIn: parent
                            text: qsTr("拖入 GLB/OBJ · 轨道旋转")
                            color: root.muted
                            font.pixelSize: 12
                        }
                    }
                }

                // Right inspector
                Rectangle {
                    Layout.preferredWidth: 300
                    Layout.fillHeight: true
                    radius: root.rCard
                    color: root.surface
                    border.color: root.line

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 16
                        spacing: 12

                        Text {
                            text: qsTr("清洗")
                            color: root.ink
                            font.pixelSize: 18
                            font.weight: Font.DemiBold
                        }
                        Text {
                            text: qsTr("选一个目标，预览后再导出")
                            color: root.muted
                            font.pixelSize: 12
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 8
                            TargetChip { text: qsTr("网页"); selected: Session.washTarget === 0; onClicked: Session.washTarget = 0 }
                            TargetChip { text: qsTr("游戏"); selected: Session.washTarget === 1; onClicked: Session.washTarget = 1 }
                            TargetChip { text: qsTr("自定义"); selected: Session.washTarget === 2; onClicked: Session.washTarget = 2 }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 8
                            StatBox {
                                Layout.fillWidth: true
                                title: qsTr("清洗前")
                                value: Session.washBefore.triangles !== undefined
                                       ? (Session.washBefore.triangles + qsTr(" 面"))
                                       : (Session.triangleCount + qsTr(" 面"))
                            }
                            StatBox {
                                Layout.fillWidth: true
                                title: qsTr("清洗后")
                                value: Session.washAfter.triangles !== undefined
                                       ? (Session.washAfter.triangles + qsTr(" 面"))
                                       : "—"
                                accented: true
                            }
                        }

                        Text {
                            Layout.fillWidth: true
                            visible: Session.washMessage.length > 0
                            text: Session.washMessage
                            color: root.muted
                            wrapMode: Text.WordWrap
                            font.pixelSize: 12
                        }

                        SoftBtn {
                            Layout.fillWidth: true
                            text: qsTr("预览清洗效果")
                            primary: true
                            onClicked: Session.previewWash()
                        }
                        SoftBtn {
                            Layout.fillWidth: true
                            text: qsTr("应用清洗")
                            onClicked: Session.applyWash()
                        }
                        SoftBtn {
                            Layout.fillWidth: true
                            text: qsTr("导出 GLB")
                            onClicked: saveDialog.open()
                        }

                        Item { Layout.fillHeight: true }

                        Rectangle {
                            Layout.fillWidth: true
                            height: 54
                            radius: 12
                            color: root.surfaceSoft
                            border.color: root.line
                            Column {
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.left: parent.left
                                anchors.leftMargin: 12
                                spacing: 2
                                Text {
                                    text: qsTr("面数 ") + Session.triangleCount
                                          + "  ·  "
                                          + qsTr("贴图 ~") + Math.round(Session.textureBytes / 1024) + " KB"
                                    color: root.ink
                                    font.pixelSize: 12
                                }
                                Text {
                                    text: Session.statusWarning.length ? Session.statusWarning : qsTr("状态良好")
                                    color: Session.statusWarning.length ? root.warn : root.muted
                                    font.pixelSize: 11
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Popup {
        id: aboutPopup
        anchors.centerIn: parent
        width: 440
        height: 240
        modal: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        background: Rectangle {
            color: root.surface
            radius: root.rCard
            border.color: root.line
        }
        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 10
            Text { text: qsTr("关于洗模型"); color: root.ink; font.weight: Font.DemiBold; font.pixelSize: 17 }
            Text {
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                color: root.muted
                lineHeight: 1.35
                text: qsTr("ModelWash 0.1 — 本地 3D 清洗工位。\n使用 Qt（LGPLv3 动态链接）。模型默认仅本地处理。")
            }
            Item { Layout.fillHeight: true }
            SoftBtn { text: qsTr("关闭"); primary: true; onClicked: aboutPopup.close() }
        }
    }

    Rectangle {
        id: toast
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 28
        width: Math.min(parent.width - 48, toastText.implicitWidth + 28)
        height: 40
        radius: 20
        color: root.surface
        border.color: root.line
        visible: opacity > 0.01
        opacity: 0
        z: 80
        property bool isError: false
        Text {
            id: toastText
            anchors.centerIn: parent
            color: toast.isError ? root.danger : root.ink
            font.pixelSize: 13
        }
        function show(msg, err) {
            toastText.text = msg
            isError = err
            opacity = 1
            hideTimer.restart()
        }
        Timer { id: hideTimer; interval: 3000; onTriggered: toast.opacity = 0 }
        Behavior on opacity { NumberAnimation { duration: 160 } }
    }

    // ----- components -----
    component SoftBtn: Rectangle {
        id: btn
        property string text
        property bool primary: false
        property bool enabled: true
        signal clicked()
        implicitHeight: 36
        implicitWidth: Math.max(72, label.implicitWidth + 24)
        radius: root.rCtl
        opacity: enabled ? 1 : 0.4
        color: primary ? (ma.containsMouse ? "#3F5FF0" : root.accent)
                       : (ma.containsMouse ? root.accentSoft : root.surfaceSoft)
        border.color: primary ? "transparent" : root.line
        Text {
            id: label
            anchors.centerIn: parent
            text: btn.text
            color: primary ? "white" : root.ink
            font.pixelSize: 13
            font.weight: Font.DemiBold
        }
        MouseArea {
            id: ma
            anchors.fill: parent
            hoverEnabled: true
            enabled: btn.enabled
            cursorShape: Qt.PointingHandCursor
            onClicked: btn.clicked()
        }
    }

    component NavIcon: Item {
        id: nav
        property string label
        property bool active: false
        property bool enabled: true
        property string tip: ""
        signal clicked()
        Layout.preferredWidth: 52
        Layout.preferredHeight: 52
        Layout.alignment: Qt.AlignHCenter
        opacity: enabled ? 1 : 0.35

        Rectangle {
            anchors.fill: parent
            radius: 14
            color: nav.active ? root.accentSoft : (nma.containsMouse ? "#F0F2FA" : "transparent")
        }
        Text {
            anchors.centerIn: parent
            width: parent.width - 6
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            text: nav.label
            color: nav.active ? root.accent : root.ink
            font.pixelSize: nav.label.length > 2 ? 11 : 14
            font.weight: Font.DemiBold
        }
        MouseArea {
            id: nma
            anchors.fill: parent
            hoverEnabled: true
            enabled: nav.enabled
            cursorShape: Qt.PointingHandCursor
            onClicked: nav.clicked()
            ToolTip.visible: tip.length > 0 && containsMouse
            ToolTip.text: tip
        }
    }

    component TargetChip: Rectangle {
        id: chip
        property string text
        property bool selected: false
        signal clicked()
        Layout.fillWidth: true
        height: 34
        radius: 17
        color: selected ? root.accentSoft : root.surfaceSoft
        border.color: selected ? root.accent : root.line
        Text {
            anchors.centerIn: parent
            text: chip.text
            color: selected ? root.accent : root.ink
            font.pixelSize: 12
            font.weight: Font.DemiBold
        }
        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: chip.clicked()
        }
    }

    component PersonaCard: Rectangle {
        property string title
        property string subtitle
        property string tag
        signal clicked()
        width: 280
        height: 168
        radius: root.rCard
        color: root.surface
        border.color: pma.containsMouse ? root.accent : root.line
        Behavior on border.color { ColorAnimation { duration: 120 } }

        Column {
            anchors.fill: parent
            anchors.margins: 22
            spacing: 10
            Text { text: tag; color: root.accent; font.pixelSize: 11; font.weight: Font.DemiBold }
            Text { text: title; color: root.ink; font.pixelSize: 18; font.weight: Font.DemiBold }
            Text {
                width: parent.width
                text: subtitle
                color: root.muted
                wrapMode: Text.WordWrap
                font.pixelSize: 13
                lineHeight: 1.35
            }
        }
        MouseArea {
            id: pma
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: parent.clicked()
        }
    }

    component StatBox: Rectangle {
        property string title
        property string value
        property bool accented: false
        height: 76
        radius: 14
        color: root.surfaceSoft
        border.color: root.line
        Column {
            anchors.left: parent.left
            anchors.leftMargin: 12
            anchors.verticalCenter: parent.verticalCenter
            spacing: 4
            Text { text: title; color: root.muted; font.pixelSize: 11 }
            Text {
                text: value
                color: accented ? root.ok : root.ink
                font.pixelSize: 15
                font.weight: Font.DemiBold
            }
        }
    }
}
