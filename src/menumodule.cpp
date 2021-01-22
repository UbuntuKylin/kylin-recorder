#include "menumodule.h"
#include "mainwindow.h"
#include "xatom-helper.h"
menuModule::menuModule(QWidget *parent = nullptr) : QWidget(parent)
{
    aboutWindow = new QWidget();
    mainlyt = new QVBoxLayout();
    titleLyt = initTitleBar();
    bodylyt = initBody();

    aboutWindow->setFixedSize(420,324);
    aboutWindow->setMinimumHeight(324);
    mainlyt->setMargin(0);
    mainlyt->addLayout(titleLyt);
    mainlyt->addLayout(bodylyt);
    mainlyt->addStretch();
    aboutWindow->setLayout(mainlyt);
    aboutWindow->setWindowTitle(tr("About"));
    init();
}

void menuModule::init(){
    initAction();
    setStyle();
}

void menuModule::initAction(){
    iconSize = QSize(30,30);
    menuButton = new QToolButton(MainWindow::mutual->titleRightWid);
//    menuButton->setIcon(QIcon::fromTheme("application-menu"));
//    menuButton->setFixedSize(iconSize);
//    menuButton->setFlat(true);
//    menuButton->setProperty("isWindowButton", 0x1);
//    menuButton->setProperty("useIconHighlightEffect",0x2);
    m_menu = new QMenu();
    QList<QAction *> actions ;
    QAction *actionSetting = new QAction(m_menu);
    actionSetting->setText(tr("Setting"));
    QAction *actionTheme = new QAction(m_menu);
    actionTheme->setText(tr("Theme"));
    QAction *actionHelp = new QAction(m_menu);
    actionHelp->setText(tr("Help"));
    QAction *actionAbout = new QAction(m_menu);
    actionAbout->setText(tr("About"));
    QAction *actionQuit = new QAction(m_menu);
    actionQuit->setText(tr("Quit"));
    actions<<actionSetting/*<<actionTheme*/<<actionHelp<<actionAbout<<actionQuit;//暂时禁掉主题切换按钮
    m_menu->addActions(actions);
//    互斥按钮组
    QMenu *themeMenu = new QMenu;
    QActionGroup *themeMenuGroup = new QActionGroup(this);
    QAction *autoTheme = new QAction("Auto",this);
    themeMenuGroup->addAction(autoTheme);
    themeMenu->addAction(autoTheme);
    autoTheme->setCheckable(true);
    QAction *lightTheme = new QAction("Light",this);
    themeMenuGroup->addAction(lightTheme);
    themeMenu->addAction(lightTheme);
    lightTheme->setCheckable(true);
    QAction *darkTheme = new QAction("Dark",this);
    themeMenuGroup->addAction(darkTheme);
    themeMenu->addAction(darkTheme);
    darkTheme->setCheckable(true);
    QList<QAction* > themeActions;
    themeActions<<autoTheme<<lightTheme<<darkTheme;
//    autoTheme->setChecked(true);
    actionTheme->setMenu(themeMenu);
    menuButton->setMenu(m_menu);
    connect(m_menu,&QMenu::triggered,this,&menuModule::triggerMenu);
    initGsetting();
    setThemeFromLocalThemeSetting(themeActions);
    themeUpdate();
    connect(themeMenu,&QMenu::triggered,this,&menuModule::triggerThemeMenu);
    connect(this,&menuModule::setSignal,MainWindow::mutual,&MainWindow::goset);
    connect(this,&menuModule::menuModuleClose,MainWindow::mutual->mainWid,&MainWindow::close);
    connect(this,&menuModule::menuModuleSetThemeStyle,MainWindow::mutual,[=](QString str){
        MainWindow::mutual->limitThemeColor = str;
        qDebug()<<"限制主题颜色为"<<MainWindow::mutual->limitThemeColor<<str;
    });
}

void menuModule::setThemeFromLocalThemeSetting(QList<QAction* > themeActions)
{
#if DEBUG_MENUMODULE
    confPath = "org.kylin-recorder-data.settings";
#endif
    m_pGsettingThemeStatus = new QGSettings(confPath.toLocal8Bit());
    QString appConf = m_pGsettingThemeStatus->get("thememode").toString();
    if("lightonly" == appConf){
        themeStatus = themeLightOnly;
        themeActions[1]->setChecked(true);   //程序gsetting中为浅色only的时候就给浅色按钮设置checked
    }else if("darkonly" == appConf){
        themeStatus = themeBlackOnly;
        themeActions[2]->setChecked(true);
    }else{
        themeStatus = themeAuto;
        themeActions[0]->setChecked(true);
    }
}

void menuModule::themeUpdate(){
    if(themeStatus == themeLightOnly)
    {
        setThemeLight();
    }else if(themeStatus == themeBlackOnly){
        setThemeDark();
    }else{
        setStyleByThemeGsetting();
    }
}

void menuModule::setStyleByThemeGsetting(){
    QString nowThemeStyle = m_pGsettingThemeData->get("styleName").toString();
    if("ukui-dark" == nowThemeStyle || "ukui-black" == nowThemeStyle)
    {
        setThemeDark();
    }else{
        setThemeLight();
    }
}

void menuModule::triggerMenu(QAction *act){


    QString str = act->text();
    if(tr("Quit") == str){
        emit menuModuleClose();               //退出
    }else if(tr("About") == str){
        aboutAction();                        //关于
    }else if(tr("Help") == str){
        helpAction();                         //帮助
    }else if(tr("Setting") == str){
        emit setSignal();                     //设置
    }
}

void menuModule::triggerThemeMenu(QAction *act){
    if(!m_pGsettingThemeStatus)
    {
        m_pGsettingThemeStatus = new QGSettings(confPath.toLocal8Bit());  //m_pGsettingThemeStatus指针重复使用避免占用栈空间
    }
    QString str = act->text();
    if("Light" == str){
        themeStatus = themeLightOnly;
        disconnect(m_pGsettingThemeData,&QGSettings::changed,this,&menuModule::dealSystemGsettingChange);
        m_pGsettingThemeStatus->set("thememode","lightonly");
        qDebug()<<m_pGsettingThemeStatus->get("thememode").toString();
//        disconnect()
        setThemeLight();
    }else if("Dark" == str){
        themeStatus = themeBlackOnly;
        disconnect(m_pGsettingThemeData,&QGSettings::changed,this,&menuModule::dealSystemGsettingChange);
        m_pGsettingThemeStatus->set("thememode","darkonly");
        qDebug()<<m_pGsettingThemeStatus->get("thememode").toString();
        setThemeDark();
    }else{
        themeStatus = themeAuto;
        m_pGsettingThemeStatus->set("thememode","auto");
        initGsetting();
//        updateTheme();
        themeUpdate();
    }
}

void menuModule::aboutAction(){
//    关于点击事件处理
    initAbout();
}

void menuModule::helpAction(){
//    帮助点击事件处理
#if DEBUG_MENUMODULE
    appName = "tools/kylin-recorder";
#endif
    DaemonDbus *ipcDbus = new DaemonDbus();
    if(!ipcDbus->daemonIsNotRunning()){
        ipcDbus->showGuide(appName);
    }
}

void menuModule::initAbout(){

    MotifWmHints hints;
    hints.flags = MWM_HINTS_FUNCTIONS|MWM_HINTS_DECORATIONS;
    hints.functions = MWM_FUNC_ALL;
    hints.decorations = MWM_DECOR_BORDER;
    XAtomHelper::getInstance()->setWindowMotifHint(aboutWindow->winId(), hints);
    aboutWindow->setAttribute(Qt::WA_ShowModal, true);//模态窗口
    //TODO:在屏幕中央显示
    QRect availableGeometry = qApp->primaryScreen()->availableGeometry();
    aboutWindow->move((availableGeometry.width()-aboutWindow->width())/2,(availableGeometry.height()- aboutWindow->height())/2);
//    aboutWindow->setStyleSheet(".QWidget{background-color:rgba(255,255,255,1);}");
    aboutWindow->show();
}

QHBoxLayout* menuModule::initTitleBar(){
    QLabel* titleIcon = new QLabel();
    QLabel* titleText = new QLabel();
    titleBtnClose = new QToolButton;
    titleIcon->setFixedSize(QSize(24,24));
#if DEBUG_MENUMODULE
    iconPath = ":/svg/svg/recording_128.svg";
    appShowingName = "kylin recorder";
#endif
    //TODO：直接从主题调图标，不会QIcon转qpixmap所以暂时从本地拿
    titleIcon->setPixmap(QPixmap::fromImage(QImage(iconPath)));

    titleIcon->setScaledContents(true);
    titleBtnClose->setFixedSize(30,30);
    titleBtnClose->setIcon(QIcon::fromTheme("window-close-symbolic"));
    titleBtnClose->setProperty("isWindowButton",0x2);
    titleBtnClose->setProperty("useIconHighlightEffect",0x8);
    titleBtnClose->setAutoRaise(true);

    titleBtnClose->setFixedSize(30,30);
    titleBtnClose->setStyleSheet("QToolButton{border-radius:4px;}"
                               "QToolButton:hover{background-color:#F86457;}"
                               "QToolButton:pressed{background-color:#E44C50;}");
    connect(titleBtnClose,&QPushButton::clicked,[=](){aboutWindow->close();});
    QHBoxLayout *hlyt = new QHBoxLayout;
    titleText->setText(tr(appShowingName.toLocal8Bit()));
    titleText->setStyleSheet("font-size:14px;");
    hlyt->setSpacing(0);
    hlyt->setMargin(4);
    hlyt->addSpacing(4);
    hlyt->addWidget(titleIcon,0,Qt::AlignBottom); //居下显示
    hlyt->addSpacing(8);
    hlyt->addWidget(titleText,0,Qt::AlignBottom);
    hlyt->addStretch();
    hlyt->addWidget(titleBtnClose,0,Qt::AlignBottom);
    return hlyt;
}

QVBoxLayout* menuModule::initBody(){
#if DEBUG_MENUMODULE
    appVersion = "2020.01.07";
#endif
    QLabel* bodyIcon = new QLabel();
    bodyIcon->setFixedSize(96,96);
    bodyIcon->setPixmap(QPixmap::fromImage(QImage(iconPath)));
    bodyIcon->setStyleSheet("font-size:14px;");
    bodyIcon->setScaledContents(true);
    QLabel* bodyAppName = new QLabel();
    bodyAppName->setFixedHeight(28);
    bodyAppName->setText(tr(appShowingName.toLocal8Bit()));
    bodyAppName->setStyleSheet("font-size:18px;");
    QLabel* bodyAppVersion = new QLabel();
    bodyAppVersion->setFixedHeight(24);
    bodyAppVersion->setText(tr("Version: ") + appVersion);
    bodyAppVersion->setAlignment(Qt::AlignLeft);
    bodyAppVersion->setStyleSheet("font-size:14px;");
    QLabel* bodySupport = new QLabel();
    bodySupport->setText(tr("Support: baibai@kylinos.cn"));
    bodySupport->setFixedHeight(24);
    bodySupport->setStyleSheet("font-size:14px;");
    QVBoxLayout *vlyt = new QVBoxLayout;
    vlyt->setMargin(0);
    vlyt->setSpacing(0);
    vlyt->addSpacing(20);
    vlyt->addWidget(bodyIcon,0,Qt::AlignHCenter);
    vlyt->addSpacing(16);
    vlyt->addWidget(bodyAppName,0,Qt::AlignHCenter);
    vlyt->addSpacing(12);
    vlyt->addWidget(bodyAppVersion,0,Qt::AlignHCenter);
    vlyt->addSpacing(12);
    vlyt->addWidget(bodySupport,0,Qt::AlignHCenter);
    vlyt->addStretch();
    return vlyt;
}

void menuModule::setStyle(){
//    menuButton->setStyleSheet("QPushButton::menu-indicator{image:None;}");
}

void menuModule::initGsetting(){
    if(QGSettings::isSchemaInstalled(FITTHEMEWINDOW)){
        m_pGsettingThemeData = new QGSettings(FITTHEMEWINDOW);
        connect(m_pGsettingThemeData,&QGSettings::changed,this,&menuModule::dealSystemGsettingChange);
    }

}

void menuModule::dealSystemGsettingChange(const QString key){
    if(key == "styleName"){
        refreshThemeBySystemConf();
    }
}

void menuModule::refreshThemeBySystemConf(){
    QString themeNow = m_pGsettingThemeData->get("styleName").toString();
    if("ukui-dark" == themeNow || "ukui-black" == themeNow){
        setThemeDark();
    }else{
        setThemeLight();
    }
}

void menuModule::setThemeDark(){
    qDebug()<<"set theme dark";
    if(aboutWindow)
    {
//        aboutWindow->setStyleSheet("background-color:rgba(31,32,34，1);");
//        titleBtnClose->setIcon(QIcon(":/svg/svg/dark-window-close.svg"));
    }
    emit menuModuleSetThemeStyle("dark-theme");
}

void menuModule::setThemeLight(){
    qDebug()<<"set theme light";
    if(aboutWindow)
    {
//        aboutWindow->setStyleSheet("background-color:rgba(255，255，255，1);");
    }
    emit menuModuleSetThemeStyle("light-theme");

}
