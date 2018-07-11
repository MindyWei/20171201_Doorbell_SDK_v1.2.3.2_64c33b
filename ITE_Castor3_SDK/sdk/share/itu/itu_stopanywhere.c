#include <assert.h>
#include <malloc.h>
#include <string.h>
#include <math.h>
#include "ite/itu.h"
#include "itu_cfg.h"
#include "itu_private.h"

#define STOPANYWHERE_TIMER_CYCLE_MAX  20         //17ms x 20 ~ 340ms
#define STOPANYWHERE_SLIDE_CHECK_TIMING  300     //300ms
#define STOPANYWHERE_SLIDE_REQUIRE_DIST  1800    //~X40 ~Y40
#define STOPANYWHERE_BOUNCE_RATIO 10 //10%

//Debug Switch List
#define STOPANYWHERE_DEBUG_SETXY 0
#define STOPANYWHERE_DEBUG_FIXXY 0

static const char sawName[] = "ITUStopAnywhere";

ITUWidget* StopAnywhereGetVisibleChild(ITUStopAnywhere* saw)
{
	ITCTree *child, *tree = (ITCTree*)saw;
	assert(tree);

	for (child = tree->child; child; child = child->sibling)
	{
		ITUWidget* widget = (ITUWidget*)child;

		if (widget->visible)
		{
			return widget;
		}
	}
	return NULL;
}

int ituStopAnywhereFixX(ITUStopAnywhere* saw, int targetX)
{
	int result_value = targetX;

	if (saw)
	{
		ITUWidget* widget = (ITUWidget*)saw;
		ITUWidget* child = StopAnywhereGetVisibleChild(saw);

		if (widget && child)
		{
			bool check_result = false;

			if (saw->bounce == 1)
			{
				int left_side_boundary = (widget->rect.width / STOPANYWHERE_BOUNCE_RATIO);
				int right_side_boundary = ((widget->rect.width * (100 - STOPANYWHERE_BOUNCE_RATIO)) / 100);

				if (targetX > 0)
				{
					saw->stopAnywhereFlags |= ITU_STOPANYWHERE_BOUNCING_L;
					saw->stopAnywhereFlags |= ITU_STOPANYWHERE_BOUNCING;

					if (targetX > left_side_boundary)
					{
						check_result = true;
						result_value = left_side_boundary;
					}
				}
				else if ((targetX + child->rect.width) < widget->rect.width)
				{
					saw->stopAnywhereFlags |= ITU_STOPANYWHERE_BOUNCING_R;
					saw->stopAnywhereFlags |= ITU_STOPANYWHERE_BOUNCING;

					if ((targetX + child->rect.width) < right_side_boundary)
					{
						check_result = true;
						result_value = (right_side_boundary - child->rect.width);
					}
				}
				else
				{
					saw->stopAnywhereFlags &= ~ITU_STOPANYWHERE_BOUNCING;
					saw->stopAnywhereFlags &= ~ITU_STOPANYWHERE_BOUNCING_L;
					saw->stopAnywhereFlags &= ~ITU_STOPANYWHERE_BOUNCING_R;
				}
			}
			else
			{
				if (targetX > 0)
				{
					if (STOPANYWHERE_DEBUG_FIXXY)
						printf("[DEBUG] Func: %s [Fix X from %d to %d]\n", __func__, targetX, 0);

					check_result = true;
					result_value = 0;
				}
				else if ((targetX + child->rect.width) < widget->rect.width)
				{
					check_result = true;
					result_value = (widget->rect.width - child->rect.width);
				}
			}

			if (check_result)
			{
				//
			}

			return result_value;
		}
		else
		{
			printf("[ERROR] Func: %s [%d]\n", __func__, __LINE__);
			return result_value;
		}
	}
	else
	{
		printf("[ERROR] Func: %s [%d]\n", __func__, __LINE__);
		return result_value;
	}
}

int ituStopAnywhereFixY(ITUStopAnywhere* saw, int targetY)
{
	int result_value = targetY;

	if (saw)
	{
		ITUWidget* widget = (ITUWidget*)saw;
		ITUWidget* child = StopAnywhereGetVisibleChild(saw);

		if (widget && child)
		{
			bool check_result = false;

			if (saw->bounce == 2)
			{
				int top_side_boundary = (widget->rect.height / STOPANYWHERE_BOUNCE_RATIO);
				int bottom_side_boundary = ((widget->rect.height * (100 - STOPANYWHERE_BOUNCE_RATIO)) / 100);

				if (targetY > 0)
				{
					saw->stopAnywhereFlags |= ITU_STOPANYWHERE_BOUNCING_T;
					saw->stopAnywhereFlags |= ITU_STOPANYWHERE_BOUNCING;

					if (targetY > top_side_boundary)
					{
						check_result = true;
						result_value = top_side_boundary;
					}
				}
				else if ((targetY + child->rect.height) < widget->rect.height)
				{
					saw->stopAnywhereFlags |= ITU_STOPANYWHERE_BOUNCING_B;
					saw->stopAnywhereFlags |= ITU_STOPANYWHERE_BOUNCING;

					if ((targetY + child->rect.height) < bottom_side_boundary)
					{
						check_result = true;
						result_value = (bottom_side_boundary - child->rect.height);
					}
				}
				else
				{
					saw->stopAnywhereFlags &= ~ITU_STOPANYWHERE_BOUNCING;
					saw->stopAnywhereFlags &= ~ITU_STOPANYWHERE_BOUNCING_T;
					saw->stopAnywhereFlags &= ~ITU_STOPANYWHERE_BOUNCING_B;
				}
			}
			else
			{
				if (targetY > 0)
				{
					if (STOPANYWHERE_DEBUG_FIXXY)
						printf("[DEBUG] Func: %s [Fix Y from %d to %d]\n", __func__, targetY, 0);

					check_result = true;
					result_value = 0;
				}
				else if ((targetY + child->rect.height) < widget->rect.height)
				{
					check_result = true;
					result_value = (widget->rect.height - child->rect.height);
				}
			}

			if (check_result)
			{
				//
			}

			return result_value;
		}
		else
		{
			printf("[ERROR] Func: %s [%d]\n", __func__, __LINE__);
			return result_value;
		}
	}
	else
	{
		printf("[ERROR] Func: %s [%d]\n", __func__, __LINE__);
		return result_value;
	}
}

void ituStopAnywhereSetXY(ITUStopAnywhere* saw, int x, int y, int line)
{
	if (saw)
	{
		ITUWidget* sawwidget = (ITUWidget*)saw;
		ITUWidget* child = StopAnywhereGetVisibleChild(saw);

		if (child)
		{
			int fixX = ituStopAnywhereFixX(saw, x);
			int fixY = ituStopAnywhereFixY(saw, y);

			ituWidgetSetPosition(child, fixX, fixY);

			if (STOPANYWHERE_DEBUG_SETXY)
				printf("[Debug][StopAnywhere %s][X %d][Y %d] [%d]\n", sawwidget->name, fixX, fixY, line);
		}
	}
}

bool ituStopAnywhereUpdate(ITUWidget* widget, ITUEvent ev, int arg1, int arg2, int arg3)
{
	bool result;
	ITUStopAnywhere* saw = (ITUStopAnywhere*)widget;
	ITUWidget* sawchild = NULL;
	assert(saw);

	sawchild = StopAnywhereGetVisibleChild(saw);

	if (sawchild == NULL)
		return false;

	result = ituWidgetUpdateImpl(widget, ev, arg1, arg2, arg3);

	if (ev == ITU_EVENT_TIMER)
	{
		if (widget->flags & ITU_DRAGGING)
		{
			if (saw->timerCycleCount < STOPANYWHERE_TIMER_CYCLE_MAX) //17ms x 14 ~ 250ms
				saw->timerCycleCount++;
			else
			{
				saw->timerCycleCount = 0;
				saw->clock = itpGetTickCount();
				saw->lastTimerX = ituWidgetGetX(sawchild);
				saw->lastTimerY = ituWidgetGetY(sawchild);
			}
		}
		else if (widget->flags & ITU_UNDRAGGING)
		{
			bool debug_msg = true;

			saw->frame += 2;

			if (saw->bounce == 1) //Horizontal bounce
			{
				if (saw->frame >= saw->totalframe)
				{
					if (saw->stopAnywhereFlags & ITU_STOPANYWHERE_BOUNCING_L)
					{
						saw->stopAnywhereFlags &= ~ITU_STOPANYWHERE_BOUNCING_L;
						ituWidgetSetX(sawchild, 0);
					}
					else if (saw->stopAnywhereFlags & ITU_STOPANYWHERE_BOUNCING_R)
					{
						saw->stopAnywhereFlags &= ~ITU_STOPANYWHERE_BOUNCING_R;
						ituWidgetSetX(sawchild, (widget->rect.width - sawchild->rect.width));
					}

					if (debug_msg)
						printf("[StopAnywhere][Undragging][X to %d][Frame %d]\n", sawchild->rect.x, saw->frame);

					saw->frame = 0;
					saw->stopAnywhereFlags &= ~ITU_STOPANYWHERE_BOUNCING;
					widget->flags &= ~ITU_UNDRAGGING;
					ituExecActions(widget, saw->actions, ITU_EVENT_STOPPED, 0);
				}
				else
				{
					int ani_pos = saw->childX;

					if (saw->stopAnywhereFlags & ITU_STOPANYWHERE_BOUNCING_L)
						ani_pos = (saw->childX * (saw->totalframe - saw->frame) / saw->totalframe);
					else if (saw->stopAnywhereFlags & ITU_STOPANYWHERE_BOUNCING_R)
						ani_pos = saw->childX + ((widget->rect.width - (saw->childX + sawchild->rect.width)) * saw->frame / saw->totalframe);

					ituWidgetSetX(sawchild, ani_pos);

					if (debug_msg)
						printf("[StopAnywhere][Undragging][X to %d][Frame %d]\n", sawchild->rect.x, saw->frame);
				}
			}
			else if (saw->bounce == 2) //Vertical bounce
			{
				if (saw->frame >= saw->totalframe)
				{
					if (saw->stopAnywhereFlags & ITU_STOPANYWHERE_BOUNCING_T)
					{
						saw->stopAnywhereFlags &= ~ITU_STOPANYWHERE_BOUNCING_T;
						ituWidgetSetY(sawchild, 0);
					}
					else if (saw->stopAnywhereFlags & ITU_STOPANYWHERE_BOUNCING_B)
					{
						saw->stopAnywhereFlags &= ~ITU_STOPANYWHERE_BOUNCING_B;
						ituWidgetSetY(sawchild, (widget->rect.height - sawchild->rect.height));
					}

					if (debug_msg)
						printf("[StopAnywhere][Undragging][Y to %d][Frame %d]\n", sawchild->rect.y, saw->frame);

					saw->frame = 0;
					saw->stopAnywhereFlags &= ~ITU_STOPANYWHERE_BOUNCING;
					widget->flags &= ~ITU_UNDRAGGING;
					ituExecActions(widget, saw->actions, ITU_EVENT_STOPPED, 0);
				}
				else
				{
					int ani_pos = saw->childY;

					if (saw->stopAnywhereFlags & ITU_STOPANYWHERE_BOUNCING_T)
						ani_pos = (saw->childY * (saw->totalframe - saw->frame) / saw->totalframe);
					else if (saw->stopAnywhereFlags & ITU_STOPANYWHERE_BOUNCING_B)
						ani_pos = saw->childY + ((widget->rect.height - (saw->childY + sawchild->rect.height)) * saw->frame / saw->totalframe);

					ituWidgetSetY(sawchild, ani_pos);

					if (debug_msg)
						printf("[StopAnywhere][Undragging][Y to %d][Frame %d]\n", sawchild->rect.y, saw->frame);
				}
			}
		}
		else if (saw->stopAnywhereFlags & ITU_STOPANYWHERE_SLIDING)
		{
			int slideX = 0;
			int slideY = 0;

			saw->frame++;
			
			slideX = saw->factorX / (saw->frame + 1);
			slideY = saw->factorY / (saw->frame + 1);

			ituStopAnywhereSetXY(saw, (saw->childX + slideX), (saw->childY + slideY), __LINE__);

			//printf("frame %d, [%d,%d] step dist %d, %d\n", saw->frame, saw->childX, saw->childY, slideX, slideY);

			if ((saw->childX == sawchild->rect.x) && (saw->childY == sawchild->rect.y))
			{
				saw->frame = saw->totalframe;
			}

			saw->childX = ituWidgetGetX(sawchild);
			saw->childY = ituWidgetGetY(sawchild);

			ituExecActions(widget, saw->actions, ITU_EVENT_CHANGED, 0);

			if (saw->frame >= saw->totalframe)
			{
				saw->stopAnywhereFlags &= ~ITU_STOPANYWHERE_SLIDING;
				saw->frame = 0;

				if (saw->stopAnywhereFlags & ITU_STOPANYWHERE_BOUNCING)
				{
					widget->flags |= ITU_UNDRAGGING;
				}
				else
					ituExecActions(widget, saw->actions, ITU_EVENT_STOPPED, 0);
			}
		}
	}
	else if (ev == ITU_EVENT_MOUSEDOWN)
	{
		if (saw->stopAnywhereFlags & ITU_STOPANYWHERE_SLIDING)
		{
			saw->stopAnywhereFlags &= ~ITU_STOPANYWHERE_SLIDING;
			saw->frame = 0;
			saw->factorX = 0;
			saw->factorY = 0;

			ituUnPressWidget(sawchild);
		}

		if (widget->flags & ITU_DRAGGABLE)
		{
			int x = arg2 - widget->rect.x;
			int y = arg3 - widget->rect.y;

			if ((ituWidgetIsInside(widget, x, y)) && (ituScene->dragged == NULL))
			{
				saw->initX = saw->touchX = x;
				saw->initY = saw->touchY = y;
				saw->lastTimerX = saw->childX = ituWidgetGetX(sawchild);
				saw->lastTimerY = saw->childY = ituWidgetGetY(sawchild);

				saw->timerCycleCount = 0;
				saw->clock = itpGetTickCount();

				widget->flags |= ITU_DRAGGING;
				ituScene->dragged = widget;
			}
		}
	}
	else if (ev == ITU_EVENT_MOUSEUP)
	{
		if (widget->flags & ITU_DRAGGING)
		{
			uint32_t timetick = itpGetTickCount();

			if ((timetick - saw->clock) <= STOPANYWHERE_SLIDE_CHECK_TIMING)
			{
				int valueX = ituWidgetGetX(sawchild) - saw->lastTimerX;
				int valueY = ituWidgetGetY(sawchild) - saw->lastTimerY;
				int lastDistX = abs(valueX);
				int lastDistY = abs(valueY);
				int lastDist = (lastDistX * lastDistX) + (lastDistY * lastDistY);

				if (lastDist > STOPANYWHERE_SLIDE_REQUIRE_DIST)
				{
					saw->factorX = valueX;
					saw->factorY = valueY;

					saw->frame = 0;
					saw->stopAnywhereFlags |= ITU_STOPANYWHERE_SLIDING;
				}
				else
				{
					printf("[1800][lastDist, lastDistX, lastDistY] [%d, %d, %d]\n", lastDist, lastDistX, lastDistY);
				}
			}
			else
				printf("[%d][Clock][Tick, saw_last_clock][%d, %d]\n", STOPANYWHERE_SLIDE_CHECK_TIMING, timetick, saw->clock);

			saw->touchX = 0;
			saw->touchY = 0;
			saw->childX = ituWidgetGetX(sawchild);
			saw->childY = ituWidgetGetY(sawchild);

			widget->flags &= ~ITU_DRAGGING;
			ituScene->dragged = NULL;

			if (!(saw->stopAnywhereFlags & ITU_STOPANYWHERE_SLIDING))
			{
				if (saw->stopAnywhereFlags & ITU_STOPANYWHERE_BOUNCING)
					widget->flags |= ITU_UNDRAGGING;
				else
					ituExecActions(widget, saw->actions, ITU_EVENT_STOPPED, 0);

				saw->frame = 0;
			}
		}
	}
	else if (ev == ITU_EVENT_MOUSEMOVE)
	{
		if (widget->flags & ITU_DRAGGING)
		{
			int x = arg2 - widget->rect.x;
			int y = arg3 - widget->rect.y;
			int distX = x - saw->touchX;
			int distY = y - saw->touchY;
			int Xtd = x - saw->initX;
			int Ytd = y - saw->initY;

			if ((abs(Xtd) >= ITU_DRAG_DISTANCE) || (abs(Ytd) >= ITU_DRAG_DISTANCE))
			{
				ituUnPressWidget(sawchild);
				//printf("[Unpress] x,y[%d,%d] init[%d,%d]\n", x, y, saw->initX, saw->initY);
			}

			if (ituWidgetIsInside(widget, x, y))
			{
				ituStopAnywhereSetXY(saw, (saw->childX + distX), (saw->childY + distY), __LINE__);
				saw->touchX = x;
				saw->touchY = y;
				saw->childX = ituWidgetGetX(sawchild);
				saw->childY = ituWidgetGetY(sawchild);
				ituWidgetSetDirty(widget, true);

				ituExecActions(widget, saw->actions, ITU_EVENT_CHANGED, 0);
			}
		}
	}

	result |= widget->dirty;

	return widget->visible ? result : false;
}

void ituStopAnywhereDraw(ITUWidget* widget, ITUSurface* dest, int x, int y, uint8_t alpha)
{
	int destx, desty;
	uint8_t desta;
	ITUStopAnywhere* saw = (ITUStopAnywhere*)widget;
	ITURectangle* rect = &widget->rect;
	ITCTree* node;
	ITURectangle prevClip;
	assert(saw);
	assert(dest);

	if (!widget->visible)
		return;

	destx = rect->x + x;
	desty = rect->y + y;
	desta = alpha * widget->alpha / 255;

	if (widget->angle == 0)
	{
		for (node = widget->tree.child; node; node = node->sibling)
		{
			ITUWidget* child = (ITUWidget*)node;

			//if ((widget->flags & ITU_DRAGGING) && (child != ituScene->dragged))
			ituWidgetSetClipping(widget, dest, x, y, &prevClip);

			if (child->visible && ituWidgetIsOverlapClipping(child, dest, destx, desty))
				ituWidgetDraw(node, dest, destx, desty, desta);

			//if ((widget->flags & ITU_DRAGGING) && (child != ituScene->dragged))
			ituSurfaceSetClipping(dest, prevClip.x, prevClip.y, prevClip.width, prevClip.height);

			child->dirty = false;
		}
	}
	else
	{
		ITUSurface* surf;

		surf = ituCreateSurface(rect->width, rect->height, 0, ITU_ARGB8888, NULL, 0);
		if (surf)
		{
			ITUColor color = { 0, 0, 0, 0 };

			ituColorFill(surf, 0, 0, rect->width, rect->height, &color);

			for (node = widget->tree.child; node; node = node->sibling)
			{
				ITUWidget* child = (ITUWidget*)node;

				if (child->visible && ituWidgetIsOverlapClipping(child, dest, destx, desty))
					ituWidgetDraw(node, surf, 0, 0, desta);

				child->dirty = false;
			}

#if (CFG_CHIP_FAMILY == 9070)
			ituRotate(dest, destx + rect->width / 2, desty + rect->height / 2, surf, surf->width / 2, surf->height / 2, (float)widget->angle, 1.0f, 1.0f);
#else
			ituRotate(dest, destx, desty, surf, surf->width / 2, surf->height / 2, (float)widget->angle, 1.0f, 1.0f);
#endif
			ituDestroySurface(surf);
		}
	}
}

void ituStopAnywhereOnAction(ITUWidget* widget, ITUActionType action, char* param)
{
	ITUStopAnywhere* saw = (ITUStopAnywhere*)widget;
	assert(saw);

	switch (action)
	{
	case ITU_ACTION_RELOAD:
		ituStopAnywhereUpdate(widget, ITU_EVENT_LAYOUT, 0, 0, 0);
		break;

	default:
		ituWidgetOnActionImpl(widget, action, param);
		break;
	}
}

void ituStopAnywhereInit(ITUStopAnywhere* saw)
{
    assert(saw);
    ITU_ASSERT_THREAD();

    memset(saw, 0, sizeof (ITUStopAnywhere));

	ituWidgetInit(&saw->widget);
    
    ituWidgetSetType(saw, ITU_STOPANYWHERE);
    ituWidgetSetName(saw, sawName);
	ituWidgetSetUpdate(saw, ituStopAnywhereUpdate);
	ituWidgetSetDraw(saw, ituStopAnywhereDraw);
	ituWidgetSetOnAction(saw, ituStopAnywhereOnAction);
}

void ituStopAnywhereLoad(ITUStopAnywhere* saw, uint32_t base)
{
    assert(saw);

	//saw->bounce = 2;

	ituWidgetLoad(&saw->widget, base);
	ituWidgetSetUpdate(saw, ituStopAnywhereUpdate);
	ituWidgetSetDraw(saw, ituStopAnywhereDraw);
	ituWidgetSetOnAction(saw, ituStopAnywhereOnAction);
}

int ituStopAnywhereGetChildX(ITUStopAnywhere* saw)
{
	assert(saw);

	return saw->childX;
}

int ituStopAnywhereGetChildY(ITUStopAnywhere* saw)
{
	assert(saw);

	return saw->childY;
}