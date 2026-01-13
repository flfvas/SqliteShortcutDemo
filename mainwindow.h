#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QtSql/QSqlDatabase>
#include <qhotkey.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // 核心业务流：由全局热键触发
    void onHandleGlobalWorkflow();

private:
    // --- 过程分离：初始化函数 ---
    void setupUI();             // 界面初始化
    void setupGlobalHotkeys();  // 全局热键初始化
    void initDatabase();        // 数据库环境初始化

    // --- 功能分离：逻辑函数 ---
    void loadPersistentData();  // 加载数据
    void updatePersistentData();// 保存数据
    void refreshInterface();    // 刷新显示

    QLabel *label;
    int Xuhao = 1;              // 对应 AHK 中的 CurrentVal [cite: 15]
    QSqlDatabase db;
    QHotkey *hotkey;            // QHotkey 对象
};

#endif
