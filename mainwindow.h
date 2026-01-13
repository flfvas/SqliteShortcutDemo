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
    void incrementXuhao(); // 处理自增逻辑

private:
    void initDatabase();   // 初始化数据库
    void loadData();       // 从数据库读取初始值
    void updateDatabase(); // 更新数据库值

    QLabel *label;
    int Xuhao = 1;
    QSqlDatabase db;
};

#endif // MAINWINDOW_H
