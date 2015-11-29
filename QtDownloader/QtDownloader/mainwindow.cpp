#include "mainwindow.h"
#include <qt_windows.h>
#include <QMouseEvent>
#include <QDesktopWidget>

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))
#endif

MainWindow::MainWindow(QWidget *parent)
	: QDialog(parent, 0)
{
	ui.setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint);
}

MainWindow::~MainWindow()
{

}

bool MainWindow::winEvent(MSG *message, long *result)
{
	if (message->message == WM_NCHITTEST)
	{
		QPoint cursorPos(GET_X_LPARAM(message->lParam), GET_Y_LPARAM(message->lParam));
		cursorPos = mapFromGlobal(cursorPos);
		*result = hitTest(cursorPos);
		return *result != HTCLIENT && *result != HTCAPTION;
	}
	else if (message->message == WM_LBUTTONDOWN)
	{
		QPoint cursorPos(GET_X_LPARAM(message->lParam), GET_Y_LPARAM(message->lParam));
		if (hitTest(cursorPos) == HTCAPTION)
		{
			PostMessage(message->hwnd, WM_SYSCOMMAND, HTCAPTION | SC_MOVE, 0);
			return true;
		}
	}
	else if (message->message == WM_LBUTTONDBLCLK)
	{
		QPoint cursorPos(GET_X_LPARAM(message->lParam), GET_Y_LPARAM(message->lParam));
		long ht = hitTest(cursorPos);
		if (ht == HTCAPTION)
		{
			onMaximize();
			return true;
		}
	}
	else if (message->message == WM_NCCALCSIZE)
	{
		if (message->wParam && IsZoomed(message->hwnd))
		{
			NCCALCSIZE_PARAMS *ncsp = reinterpret_cast<NCCALCSIZE_PARAMS*>(message->lParam);
			QDesktopWidget desktopWidget;
			auto rect = desktopWidget.availableGeometry(this);
			ncsp->rgrc[0].left = rect.left();
			ncsp->rgrc[0].top = rect.top();
			ncsp->rgrc[0].right = rect.right();
			ncsp->rgrc[0].bottom = rect.bottom();
		}
		return true;
	}
	return false;
}

void MainWindow::showEvent(QShowEvent *e)
{
	//DON'T set window style in constructor, cause when layout is applied, window style you set in constructor will be changed.
	HWND hWnd = reinterpret_cast<HWND>(winId());
	SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) | WS_CAPTION | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX);
	__super::showEvent(e);
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
	if (e->key() != Qt::Key_Escape)
	{
		__super::keyPressEvent(e);
	}
}

void MainWindow::onMinimize()
{
	PostMessage(reinterpret_cast<HWND>(winId()), WM_SYSCOMMAND, SC_MINIMIZE, 0);
}

void MainWindow::onMaximize()
{
	HWND hWnd = reinterpret_cast<HWND>(winId());
	if (IsZoomed(hWnd))
	{
		PostMessage(hWnd, WM_SYSCOMMAND, HTCAPTION | SC_RESTORE, 0);
	} 
	else
	{
		PostMessage(hWnd, WM_SYSCOMMAND, HTCAPTION | SC_MAXIMIZE, 0);
	}
}

long MainWindow::hitTest(const QPoint &pos)
{
	if (IsZoomed(reinterpret_cast<HWND>(winId())))
	{
		auto child = childAt(pos);
		if (!child || child == this)
		{
			return HTCAPTION;
		}
		return HTCLIENT;
	}
	bool left = pos.x() <= 5;
	bool top = pos.y() <= 5;
	bool right = width() - pos.x() <= 5;
	bool bottom = height() - pos.y() <= 5;
	if (left && top)
	{
		return HTTOPLEFT;
	}
	else if (left && bottom)
	{
		return HTBOTTOMLEFT;
	}
	else if (right && top)
	{
		return HTTOPRIGHT;
	}
	else if (right && bottom)
	{
		return HTBOTTOMRIGHT;
	}
	else if (left)
	{
		return HTLEFT;
	}
	else if (top)
	{
		return HTTOP;
	}
	else if (right)
	{
		return HTRIGHT;
	}
	else if (bottom)
	{
		return HTBOTTOM;
	}
	else
	{
		auto child = childAt(pos);
		 if (!child || child == this)
		 {
			 return HTCAPTION;
		 }
	}
	return HTCLIENT;
}