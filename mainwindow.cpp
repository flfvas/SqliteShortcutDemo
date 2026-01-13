#include "mainwindow.h"
#include <QVBoxLayout>
#include <QSqlError>
#include <QSqlQuery>
#include <QDateTime>
#include <QApplication>
#include <QClipboard>
#include <QStatusBar>

/**
 * 构造函数：过程与功能分离
 * 前几行仅负责调用各个功能模块
 */
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setupUI();              // 1. 设置界面
    setupGlobalHotkeys();   // 2. 绑定 Ctrl+Q
    initDatabase();         // 3. 连接数据库
    loadPersistentData();   // 4. 同步初始序号
}

MainWindow::~MainWindow()
{
    if (db.isOpen()) db.close(); // 安全关闭数据库 [cite: 23]
}

// --- 第一部分：快捷键触发的核心业务流 ---

/**
 * 模拟 AHK *$#C:: 热键逻辑
 */
void MainWindow::onHandleGlobalWorkflow()
{
    // A. 格式化日期 yyyyMMdd-HHmmss [cite: 16]
    QString timeStr = QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss");

    // B. 组合字符串 (时间-序号) [cite: 17]
    QString result = QString("%1-%2").arg(timeStr).arg(Xuhao);

    // C. 写入剪贴板 [cite: 16]
    QApplication::clipboard()->setText(result);

    // D. 序号自增并持久化 [cite: 19]
    Xuhao++;
    updatePersistentData();
    refreshInterface();

    // E. 状态栏提示 (替代 AHK ToolTip) [cite: 22]
    statusBar()->showMessage(QString("已更新并复制: %1").arg(result), 2000);
}

// --- 第二部分：各功能模块具体实现 ---

void MainWindow::setupGlobalHotkeys()
{
    // 注册全局热键 Ctrl+Q (对应 AHK 的 *$#C) [cite: 12]
    hotkey = new QHotkey(QKeySequence("Ctrl+Q"), true, this);

    // 连接热键信号到业务流函数
    connect(hotkey, &QHotkey::activated, this, &MainWindow::onHandleGlobalWorkflow);
}

void MainWindow::initDatabase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("CounterData.db"); // 对标 AHK 数据库名 [cite: 8]

    if (db.open()) {
        QSqlQuery q;
        // 创建表 [cite: 10]
        q.exec("CREATE TABLE IF NOT EXISTS MyCounter (id INTEGER PRIMARY KEY, val INTEGER);");
        // 初始化数据 [cite: 11]
        q.exec("INSERT INTO MyCounter (id, val) SELECT 1, 1 WHERE NOT EXISTS (SELECT 1 FROM MyCounter WHERE id=1);");
    }
}

void MainWindow::loadPersistentData()
{
    QSqlQuery q("SELECT val FROM MyCounter WHERE id=1;");
        if (q.next()) {
        Xuhao = q.value(0).toInt(); // 获取当前序号 [cite: 15]
    }
    refreshInterface();
}

void MainWindow::updatePersistentData()
{
    QSqlQuery q;
    q.prepare("UPDATE MyCounter SET val = ? WHERE id = 1;");
        q.addBindValue(Xuhao);
    q.exec();
}

void MainWindow::setupUI()
{
    QWidget *cw = new QWidget(this);
    QVBoxLayout *lay = new QVBoxLayout(cw);

    label = new QLabel(this);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("font-size: 22px; font-family: 'Consolas'; color: #2980b9;");

    lay->addWidget(label);
    setCentralWidget(cw);
    setWindowTitle("全局序号生成器");
    resize(320, 160);
}

void MainWindow::refreshInterface()
{
    label->setText(QString("当前序号: %1").arg(Xuhao));
}
