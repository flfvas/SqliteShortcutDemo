#include "mainwindow.h"
#include <QVBoxLayout>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QDateTime>
#include <QApplication>
#include <QClipboard>
#include <QStatusBar>

/**
 * 快捷键配置模块
 */
void MainWindow::setupShortcuts()
{
    // 定义快捷键序列（Ctrl+Shift+C），这在大多数场景下不会与系统冲突
    QShortcut *shortcut = new QShortcut(QKeySequence("Ctrl+Shift+C"), this);

    // 建立信号槽连接：当用户按下快捷键时，执行 onShortcutActivated 函数
    connect(shortcut, &QShortcut::activated, this, &MainWindow::onShortcutActivated);
}




// 核心业务槽函数：快捷键触发入口    逻辑对标 AHK 的 *$#C 流程
void MainWindow::onShortcutActivated()
{
    // 步骤 A：生成当前时间序列字符串
    // 使用 yyyyMMdd-HHmmss 格式，这种格式在文件名中非常友好
    QString timeStr = QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss");

    // 步骤 B：字符串拼接
    // 将“时间戳”与“内存中的当前序号”合并，例如：20231027-143005-1
    QString result = QString("%1-%2").arg(timeStr).arg(Xuhao);

    // 步骤 C：操作系统剪贴板
    // 将生成的字符串直接推送到剪贴板，用户可以直接在任何地方粘贴
    QApplication::clipboard()->setText(result);

    // 步骤 D：触发数据演变逻辑
    // 该函数负责：1. 增加序号 2. 刷新界面显示 3. 写入数据库
    incrementAndSave();

    // 步骤 E：视觉反馈
    // 在状态栏显示临时消息，停留 3000 毫秒，模仿 AHK 的 ToolTip 效果
    statusBar()->showMessage(QString("已存入剪贴板: %1 | 数据库已更新").arg(result), 3000);
}





/*
* 构造函数 (Constructor)
* 职责：仅作为各个独立功能模块的“调度员”。
* 这种结构将程序的“启动流程”与“具体实现”完全解耦，易于阅读和后期维护。
*/
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();            // 第一步：构建静态 UI 界面
    setupShortcuts();     // 第二步：配置全局/局部快捷键监听
    initDatabase();       // 第三步：建立数据库连接并检查表结构
    loadData();           // 第四步：从本地数据库同步最新的序号数据到内存
}


// 析构函数 (Destructor)   职责：安全退出。
MainWindow::~MainWindow()
{
    // 程序关闭前确保释放数据库句柄，防止数据库文件被占用锁定
    if (db.isOpen()) {
        db.close();
    }
}



// =================================================================
// 功能模块实现区
// =================================================================



// 数据库初始化模块
void MainWindow::initDatabase()
{
    // 加载 SQLite 驱动并指定数据库文件路径（当前运行目录下）
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("CounterData.db");

    // 尝试打开数据库，若失败则输出调试信息并停止初始化
    if (!db.open()) {
        qCritical() << "CRITICAL ERROR: 无法打开数据库文件！" << db.lastError().text();
        return;
    }

    QSqlQuery query;
    // 创建计数表：如果表不存在则创建，id 为 1 的行将始终存储我们的计数器
    QString createTableSql = "CREATE TABLE IF NOT EXISTS MyCounter (id INTEGER PRIMARY KEY, val INTEGER);";
    if (!query.exec(createTableSql)) {
        qDebug() << "SQL Error (CreateTable):" << query.lastError().text();
    }

    // 初始化检查：如果表里没有任何数据，则插入第一行数据，初始值为 1
    QString initRowSql = "INSERT INTO MyCounter (id, val) SELECT 1, 1 WHERE NOT EXISTS (SELECT 1 FROM MyCounter WHERE id=1);";
    query.exec(initRowSql);
}


// 数据读取模块
void MainWindow::loadData()
{
    // 从数据库中提取当前生效的序号值
    QSqlQuery query("SELECT val FROM MyCounter WHERE id=1;");
    if (query.next()) {
        // 将查询结果转换为整数并存储在类成员变量 Xuhao 中
        Xuhao = query.value(0).toInt();
    }
    // 刷新界面上的 Label 文本
    updateLabel();
}

/**
 * 业务数据保存模块
 */
void MainWindow::incrementAndSave()
{
    // 1. 执行内存中的变量自增
    Xuhao++;

    // 2. 将自增后的最新值同步到 SQLite 数据库中，实现持久化存储
    QSqlQuery query;
    query.prepare("UPDATE MyCounter SET val = :newVal WHERE id = 1;");
    query.bindValue(":newVal", Xuhao);

    if (!query.exec()) {
        qDebug() << "SQL Error (Update):" << query.lastError().text();
    }

    // 3. 数据更新后，立即同步刷新 UI 显示
    updateLabel();
}


/**
 * 用户界面初始化模块
 */
void MainWindow::setupUI()
{
    // 创建中心容器
    QWidget *centralWidget = new QWidget(this);
    // 使用垂直布局
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    // 初始化标签控件
    label = new QLabel(this);
    label->setAlignment(Qt::AlignCenter);
    // 使用样式表美化：加大字体并设置 Consolas 字体（等宽字体看数字更整齐）
    label->setStyleSheet("font-size: 22px; font-family: 'Consolas'; font-weight: bold; color: #2980b9;");

    // 将标签加入布局并设置到主窗口
    layout->addWidget(label);
    setCentralWidget(centralWidget);

    // 设置窗口基本属性
    setWindowTitle("序号生成器 v1.0");
    resize(320, 160);
}

/**
 * 界面刷新模块
 */
void MainWindow::updateLabel()
{
    // 纯粹的 UI 更新函数，不包含任何业务逻辑，只负责显示内存变量的当前值
    label->setText(QString("下次序号: %1").arg(Xuhao));
}
