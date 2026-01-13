#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QtSql/QSqlDatabase>
#include <QShortcut>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // 处理快捷键触发的核心业务流 (对应 AHK 的 *$#C 逻辑)
    void onShortcutActivated();

private:
    // 功能步骤分离
    void setupUI();            // 初始化界面布局
    void setupShortcuts();     // 初始化快捷键绑定
    void initDatabase();       // 初始化数据库连接
    void loadData();           // 加载初始序号
    void incrementAndSave();   // 执行自增并持久化
    void updateLabel();        // 仅刷新界面文本

    QLabel *label;
    int Xuhao = 1;             // 对应 AHK 中的 CurrentVal
    QSqlDatabase db;
};

#endif
