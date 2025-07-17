#include "mainwindow.h"
#include <QApplication>
#include <QGraphicsScene>
#include "gamecontroller.h"
#include <QSplashScreen>
#include <QTimer>
#include <QPixmap>
#include <QLabel>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Step 1: 显示 Qt 启动画面（带黑色背景）
    QSplashScreen* splash = new QSplashScreen(QPixmap(":/images/Qt_logo.png"), Qt::WindowStaysOnTopHint);
    // 设置纯黑背景
    QPalette palette;
    palette.setColor(QPalette::Window, Qt::black);
    splash->setPalette(palette);
    splash->setAutoFillBackground(true);

    splash->show();

    // 添加渐入动画
    splash->setWindowOpacity(0);
    QPropertyAnimation *fadeIn = new QPropertyAnimation(splash, "windowOpacity");
    fadeIn->setDuration(500);
    fadeIn->setStartValue(0);
    fadeIn->setEndValue(1);
    fadeIn->start();

    a.processEvents();
    splash->showMessage("Loading...", Qt::AlignBottom | Qt::AlignHCenter, Qt::white);
    a.processEvents();

    // 初始化游戏相关对象
    QGraphicsScene* scene = new QGraphicsScene;
    GameController* game = new GameController(*scene);
    MainWindow* w = new MainWindow(*scene, nullptr, game);
    w->setWindowOpacity(0);

    // Step 2: 延时显示 "Snake" 字样
    QTimer::singleShot(2000, [&]() {
        // 渐出 splash
        QPropertyAnimation *fadeOut = new QPropertyAnimation(splash, "windowOpacity");
        fadeOut->setDuration(500);
        fadeOut->setStartValue(1);
        fadeOut->setEndValue(0);

        // 创建 SNAKE 标签（纯黑背景）
        QLabel* label = new QLabel;
        label->setText("SNAKE");
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("QLabel { "
                             "font-size: 64px; "
                             "color: white; "
                             "background-color: black; "
                             "}");
        label->setWindowFlags(Qt::SplashScreen | Qt::FramelessWindowHint);
        label->setAttribute(Qt::WA_TranslucentBackground, false);
        label->setWindowOpacity(0);

        // 标签动画组（渐入）
        QPropertyAnimation *labelFadeIn = new QPropertyAnimation(label, "windowOpacity");
        labelFadeIn->setDuration(800);
        labelFadeIn->setStartValue(0);
        labelFadeIn->setEndValue(1);
        labelFadeIn->setEasingCurve(QEasingCurve::OutBack);

        // 连接动画信号
        fadeOut->start();
        QObject::connect(fadeOut, &QPropertyAnimation::finished, [=]() {
            splash->clearMessage();
            splash->setPixmap(QPixmap());
            label->showFullScreen();
            labelFadeIn->start();
        });

        // 2秒后切换到主窗口
        QTimer::singleShot(2000, [=]() {
            // 确保主窗口在动画前设置为全屏
            w->showFullScreen();
            w->setWindowOpacity(0);

            // 主窗口动画组
            QParallelAnimationGroup *windowAnimGroup = new QParallelAnimationGroup;

            // 主窗口渐入
            QPropertyAnimation *windowFadeIn = new QPropertyAnimation(w, "windowOpacity");
            windowFadeIn->setDuration(800);
            windowFadeIn->setStartValue(0);
            windowFadeIn->setEndValue(1);

            windowAnimGroup->addAnimation(windowFadeIn);

            // 标签渐出
            QPropertyAnimation *labelFadeOut = new QPropertyAnimation(label, "windowOpacity");
            labelFadeOut->setDuration(500);
            labelFadeOut->setStartValue(1);
            labelFadeOut->setEndValue(0);

            // 连接动画信号
            labelFadeOut->start();
            windowAnimGroup->start();

            QObject::connect(windowAnimGroup, &QParallelAnimationGroup::finished, [=]() {
                // 确保主窗口保持全屏
                w->showFullScreen();
            });

            QObject::connect(labelFadeOut, &QPropertyAnimation::finished, [=]() {
                label->close();
                label->deleteLater();
                splash->finish(w);
                splash->deleteLater();
            });
        });
    });

    return a.exec();
}
