#include <assert.h>
#include <math.h>
#include "ite/itu.h"
#include "itu_cfg.h"
#include "itu_private.h"

static const char scaleCoverFlowName[] = "ITUScaleCoverFlow";

extern int CoverFlowGetVisibleChildCount(ITUCoverFlow* coverflow);
extern ITUWidget* CoverFlowGetVisibleChild(ITUCoverFlow* coverflow, int index);

bool ituScaleCoverFlowUpdate(ITUWidget* widget, ITUEvent ev, int arg1, int arg2, int arg3)
{
    bool result = false;
    ITUCoverFlow* coverflow = (ITUCoverFlow*) widget;
    ITUScaleCoverFlow* scalecoverflow = (ITUScaleCoverFlow*) widget;
    assert(scalecoverflow);

    if (ev == ITU_EVENT_TIMER && coverflow->inc)
    {
        result = true;
    }
    else if (((ev == ITU_EVENT_TOUCHSLIDELEFT || ev == ITU_EVENT_TOUCHSLIDERIGHT) && ((coverflow->coverFlowFlags & ITU_COVERFLOW_VERTICAL) == 0)) ||
             ((ev == ITU_EVENT_TOUCHSLIDEUP || ev == ITU_EVENT_TOUCHSLIDEDOWN) && (coverflow->coverFlowFlags & ITU_COVERFLOW_VERTICAL)))
    {
		if (ituWidgetIsEnabled(widget)) // && (widget->flags & ITU_DRAGGABLE))
		{
			int x = arg2 - widget->rect.x;
			int y = arg3 - widget->rect.y;

			if (ituWidgetIsInside(widget, x, y))
            {
                result = true;
            }
        }
    }
    else if (ev == ITU_EVENT_LAYOUT)
    {
        if (scalecoverflow->itemWidth > 0 || scalecoverflow->itemHeight > 0 || scalecoverflow->itemPos > 0)
        {
            int i, count = CoverFlowGetVisibleChildCount(coverflow);

            for (i = 0; i < count; ++i)
            {
                ITUWidget* child = CoverFlowGetVisibleChild(coverflow, i);

                if (coverflow->coverFlowFlags & ITU_COVERFLOW_VERTICAL)
                    ituWidgetSetX(child, scalecoverflow->itemPos);
                else
                    ituWidgetSetY(child, scalecoverflow->itemPos);

                ituWidgetSetDimension(child, scalecoverflow->itemWidth, scalecoverflow->itemHeight);
            }            
        }
    }

    if (result)
    {
        int i, count = CoverFlowGetVisibleChildCount(coverflow);

        for (i = 0; i < count; ++i)
        {
            ITUWidget* child = CoverFlowGetVisibleChild(coverflow, i);

            if (coverflow->coverFlowFlags & ITU_COVERFLOW_VERTICAL)
                ituWidgetSetX(child, scalecoverflow->itemPos);
            else
                ituWidgetSetY(child, scalecoverflow->itemPos);

            ituWidgetSetDimension(child, scalecoverflow->itemWidth, scalecoverflow->itemHeight);
        }
    }

    result |= ituCoverFlowUpdate(widget, ev, arg1, arg2, arg3);

    if (ev == ITU_EVENT_TIMER)
    {
        if (coverflow->inc)
        {
            int i, count = CoverFlowGetVisibleChildCount(coverflow);

            if (coverflow->coverFlowFlags & ITU_COVERFLOW_CYCLE)
            {
                int index, count2, orgWidth, orgHeight, factor, width, height, x, y, itemCount2;
                int frame = coverflow->frame - 1;

                count2 = count / 2;
                if (coverflow->inc < 0)
                    count2++;

                index = coverflow->focusIndex;
                itemCount2 = scalecoverflow->itemCount / 2;
                if (coverflow->inc < 0)
                    itemCount2++;

                orgWidth = scalecoverflow->itemWidth;
                orgHeight = scalecoverflow->itemHeight;

                for (i = 0; i < count2; ++i)
                {
                    ITUWidget* child = CoverFlowGetVisibleChild(coverflow, index);

                    if (index == coverflow->focusIndex)
                    {
						float step = (float)frame / coverflow->totalframe;
						step = step - 1;
						step = step * step * step + 1;
						factor = (int)(100 - (100 - scalecoverflow->factor) * step);

                        width = orgWidth * factor / 100;
                        height = orgHeight * factor / 100;
                        x = child->rect.x + (orgWidth - width) / 2;
                        y = child->rect.y + (orgHeight - height) / 2;
                        ituWidgetSetPosition(child, x, y);
                        ituWidgetSetDimension(child, width, height);
                    }
                    else if (i == itemCount2)
                    {
						float step = (float)frame / coverflow->totalframe;
						step = step - 1;
						step = step * step * step + 1;

						if (coverflow->inc > 0)
                        {
                            int current_factor = scalecoverflow->factor;
                            int next_factor = scalecoverflow->factor / 2;
							factor = (int)(current_factor - (current_factor - next_factor) * step);
                        }
                        else
                        {
                            int current_factor = scalecoverflow->factor / 2;
                            int next_factor = scalecoverflow->factor;
							factor = (int)(current_factor + (next_factor - current_factor) * step);
                            child->flags &= ~ITU_PROGRESS;
                        }
                        child->flags |= ITU_CLIP_DISABLED;
                        
                        width = orgWidth * factor / 100;
                        height = orgHeight * factor / 100;

                        if (coverflow->coverFlowFlags & ITU_COVERFLOW_VERTICAL)
                        {
                            x = child->rect.x + (orgWidth - width) / 2;
                            y = widget->rect.height / 2 - orgHeight / 2;
                            
                            if (coverflow->inc > 0)
                                y += orgHeight * i;
                            else
                                y += orgHeight * (i - 1);

                            y += (orgHeight - height) / 2;
                        }
                        else
                        {
                            x = widget->rect.width / 2 - orgWidth / 2;
                            
                            if (coverflow->inc > 0)
                                x += orgWidth * i;
                            else
                                x += orgWidth * (i - 1);

                            x += (orgWidth - width) / 2;
                            y = child->rect.y + (orgHeight - height) / 2;
                        }
                        ituWidgetSetPosition(child, x, y);
                        ituWidgetSetDimension(child, width, height);
                    }
                    else if ((child->flags & ITU_PROGRESS) == 0)
                    {
                        int focusIndex2;

                        if (coverflow->inc > 0)
                        {
                            if (coverflow->focusIndex == 0)
                                focusIndex2 = count - 1;
                            else
                                focusIndex2 = coverflow->focusIndex - 1;

							if (index == focusIndex2)
							{
								float step = (float)frame / coverflow->totalframe;
								step = step - 1;
								step = step * step * step + 1;

								factor = (int)(scalecoverflow->factor + (100 - scalecoverflow->factor) * step);
							}
                            else
                                factor = scalecoverflow->factor;
                        }
                        else
                        {
                            if (coverflow->focusIndex == count - 1)
                                focusIndex2 = 0;
                            else
                                focusIndex2 = coverflow->focusIndex + 1;

							if (index == focusIndex2)
							{
								float step = (float)frame / coverflow->totalframe;
								step = step - 1;
								step = step * step * step + 1;

								factor = (int)(scalecoverflow->factor + (100 - scalecoverflow->factor) * step);
							}
                            else
                                factor = scalecoverflow->factor;
                        }
                        width = orgWidth * factor / 100;
                        height = orgHeight * factor / 100;
                        x = child->rect.x + (orgWidth - width) / 2;
                        y = child->rect.y + (orgHeight - height) / 2;
                        ituWidgetSetPosition(child, x, y);
                        ituWidgetSetDimension(child, width, height);
                    }

                    if (index >= count - 1)
                        index = 0;
                    else
                        index++;
                }

                count2 = count - count2;
                itemCount2 = scalecoverflow->itemCount - itemCount2;
                for (i = 0; i < count2; ++i)
                {
                    ITUWidget* child = CoverFlowGetVisibleChild(coverflow, index);
                    int width, height, x, y;

                    if (i == count2 - itemCount2)
                    {
						float step = (float)frame / coverflow->totalframe;
						step = step - 1;
						step = step * step * step + 1;

                        if (coverflow->inc > 0)
                        {
                            int current_factor = scalecoverflow->factor / 2;
                            int next_factor = scalecoverflow->factor;
							factor = (int)(current_factor + (next_factor - current_factor) * step);
                            child->flags &= ~ITU_PROGRESS;
                        }
                        else
                        {
                            int current_factor = scalecoverflow->factor;
                            int next_factor = scalecoverflow->factor / 2;
							factor = (int)(current_factor - (current_factor - next_factor) * step);
                        }
                        child->flags |= ITU_CLIP_DISABLED;

                        width = orgWidth * factor / 100;
                        height = orgHeight * factor / 100;

                        if (coverflow->coverFlowFlags & ITU_COVERFLOW_VERTICAL)
                        {
                            x = child->rect.x + (orgWidth - width) / 2;
                            y = widget->rect.height / 2 - orgHeight / 2;

                            if (coverflow->inc > 0)
                                y -= orgHeight * (itemCount2 - 1);
                            else
                                y -= orgHeight * itemCount2;

                            y += (orgHeight - height) / 2;
                        }
                        else
                        {
                            x = widget->rect.width / 2 - orgWidth / 2;

                            if (coverflow->inc > 0)
                                x -= orgWidth * (itemCount2 - 1);
                            else
                                x -= orgWidth * itemCount2;

                            x += (orgWidth - width) / 2;
                            y = child->rect.y + (orgHeight - height) / 2;
                        }
                        ituWidgetSetPosition(child, x, y);
                        ituWidgetSetDimension(child, width, height);
                    }
                    else if ((child->flags & ITU_PROGRESS) == 0)
                    {
                        int focusIndex2;

                        if (coverflow->inc > 0)
                        {
                            if (coverflow->focusIndex == 0)
                                focusIndex2 = count - 1;
                            else
                                focusIndex2 = coverflow->focusIndex - 1;

							if (index == focusIndex2)
							{
								float step = (float)frame / coverflow->totalframe;
								step = step - 1;
								step = step * step * step + 1;

								factor = (int)(scalecoverflow->factor + (100 - scalecoverflow->factor) * step);
							}
                            else
                                factor = scalecoverflow->factor;
                        }
                        else
                        {
                            if (coverflow->focusIndex == count - 1)
                                focusIndex2 = 0;
                            else
                                focusIndex2 = coverflow->focusIndex + 1;

							if (index == focusIndex2)
							{
								float step = (float)frame / coverflow->totalframe;
								step = step - 1;
								step = step * step * step + 1;

								factor = (int)(scalecoverflow->factor + (100 - scalecoverflow->factor) * step);
							}
                            else
                                factor = scalecoverflow->factor;
                        }
                        width = orgWidth * factor / 100;
                        height = orgHeight * factor / 100;
                        x = child->rect.x + (orgWidth - width) / 2;
                        y = child->rect.y + (orgHeight - height) / 2;
                        ituWidgetSetPosition(child, x, y);
                        ituWidgetSetDimension(child, width, height);
                    }

                    if (index >= count - 1)
                        index = 0;
                    else
                        index++;
                }
            }
        }
    }
    else if (ev == ITU_EVENT_LAYOUT)
    {
        int i, orgWidth, orgHeight, count = CoverFlowGetVisibleChildCount(coverflow);

        if (scalecoverflow->itemWidth == 0 && scalecoverflow->itemHeight == 0 && scalecoverflow->itemPos == 0)
        {
            ITUWidget* child = CoverFlowGetVisibleChild(coverflow, coverflow->focusIndex);
            scalecoverflow->itemWidth = child->rect.width;
            scalecoverflow->itemHeight = child->rect.height;

            if (coverflow->coverFlowFlags & ITU_COVERFLOW_VERTICAL)
                scalecoverflow->itemPos = child->rect.x;
            else
                scalecoverflow->itemPos = child->rect.y;
        }
        orgWidth = scalecoverflow->itemWidth;
        orgHeight = scalecoverflow->itemHeight;

        if (coverflow->coverFlowFlags & ITU_COVERFLOW_CYCLE)
        {
            int index, count2, factor, width, height, x, y, itemCount2;
            
            count2 = count / 2 + 1;
            index = coverflow->focusIndex;
            itemCount2 = scalecoverflow->itemCount / 2 + 1;

            for (i = 0; i < count2; ++i)
            {
                ITUWidget* child = CoverFlowGetVisibleChild(coverflow, index);

                if (i < itemCount2)
                {
                    if (index == coverflow->focusIndex)
                        factor = 100;
                    else
                        factor = scalecoverflow->factor;

                    width = orgWidth * factor / 100;
                    height = orgHeight * factor / 100;
                    x = child->rect.x + (orgWidth - width) / 2;
                    y = child->rect.y + (orgHeight - height) / 2;
                    ituWidgetSetPosition(child, x, y);
                    ituWidgetSetDimension(child, width, height);
                    child->flags &= ~ITU_PROGRESS;
                }
                else
                {
                    child->flags |= ITU_PROGRESS;
                }
                child->flags &= ~ITU_CLIP_DISABLED;

                if (index >= count - 1)
                    index = 0;
                else
                    index++;
            }

            count2 = count - count2;
            itemCount2 = scalecoverflow->itemCount - itemCount2;
            for (i = 0; i < count2; ++i)
            {
                ITUWidget* child = CoverFlowGetVisibleChild(coverflow, index);

                if (i >= count2 - itemCount2)
                {
                    factor = scalecoverflow->factor;

                    width = orgWidth * factor / 100;
                    height = orgHeight * factor / 100;
                    x = child->rect.x + (orgWidth - width) / 2;
                    y = child->rect.y + (orgHeight - height) / 2;
                    ituWidgetSetPosition(child, x, y);
                    ituWidgetSetDimension(child, width, height);
                    child->flags &= ~ITU_PROGRESS;
                }
                else
                {
                    child->flags |= ITU_PROGRESS;
                }
                child->flags &= ~ITU_CLIP_DISABLED;

                if (index >= count - 1)
                    index = 0;
                else
                    index++;
            }
        }
    }

    return widget->visible ? result : false;
}

void ituScaleCoverFlowDraw(ITUWidget* widget, ITUSurface* dest, int x, int y, uint8_t alpha)
{
    ITUCoverFlow* coverflow = (ITUCoverFlow*)widget;
    ITURectangle prevClip;
    ITURectangle* rect = (ITURectangle*) &widget->rect;
    ITCTree* node;
    int count, index, count2, i;
    assert(widget);
    assert(dest);
   
    ituWidgetSetClipping(widget, dest, x, y, &prevClip);

    x += widget->rect.x;
    y += widget->rect.y;
    alpha = alpha * widget->alpha / 255;

    for (node = widget->tree.child; node; node = node->sibling)
    {
        ITUWidget* child = (ITUWidget*)node;
        if (child->visible && !(child->flags & ITU_PROGRESS) && (child->flags & ITU_CLIP_DISABLED) && ituWidgetIsOverlapClipping(child, dest, x, y))
            ituWidgetDraw(node, dest, x, y, alpha);

        child->dirty = false;
    }

    count = CoverFlowGetVisibleChildCount(coverflow);
    count2 = count / 2;
    if ((coverflow->inc < 0 && coverflow->frame < coverflow->totalframe / 2) ||
        (coverflow->inc >= 0 && coverflow->frame >= coverflow->totalframe / 2))
        count2++;

    if (coverflow->focusIndex - count2 >= 0)
        index = coverflow->focusIndex - count2;
    else
        index = count - 1 - (count2 - 1 - coverflow->focusIndex);

    //printf("focus=%d start=%d ", coverflow->focusIndex, index);

    for (i = 0; i < count2; ++i)
    {
        ITUWidget* child = CoverFlowGetVisibleChild(coverflow, index);
        if (child->visible && !(child->flags & ITU_PROGRESS) && !(child->flags & ITU_CLIP_DISABLED) && ituWidgetIsOverlapClipping(child, dest, x, y))
            ituWidgetDraw(child, dest, x, y, alpha);

        child->dirty = false;

        if (++index >= count)
            index = 0;
    }

    count2 = count - count2;

    if (coverflow->focusIndex + count2 - 1 < count)
        index = coverflow->focusIndex + count2 - 1;
    else
        index = count2 - 1 - (count - coverflow->focusIndex);

    //printf("end=%d\n", index);

    for (i = 0; i < count2; ++i)
    {
        ITUWidget* child = CoverFlowGetVisibleChild(coverflow, index);
        if (child->visible && !(child->flags & ITU_PROGRESS) && !(child->flags & ITU_CLIP_DISABLED) && ituWidgetIsOverlapClipping(child, dest, x, y))
            ituWidgetDraw(child, dest, x, y, alpha);

        child->dirty = false;

        if (--index < 0)
            index = count - 1;
    }

    ituSurfaceSetClipping(dest, prevClip.x, prevClip.y, prevClip.width, prevClip.height);
    ituDirtyWidget(widget, false);
}

void ituScaleCoverFlowInit(ITUScaleCoverFlow* scaleCoverFlow, ITULayout layout)
{
    assert(scaleCoverFlow);

    memset(scaleCoverFlow, 0, sizeof (ITUScaleCoverFlow));

    ituCoverFlowInit(&scaleCoverFlow->coverFlow, layout);

    ituWidgetSetType(scaleCoverFlow, ITU_IMAGECOVERFLOW);
    ituWidgetSetName(scaleCoverFlow, scaleCoverFlowName);
    ituWidgetSetUpdate(scaleCoverFlow, ituScaleCoverFlowUpdate);
    ituWidgetSetDraw(scaleCoverFlow, ituScaleCoverFlowDraw);
}

void ituScaleCoverFlowLoad(ITUScaleCoverFlow* scaleCoverFlow, uint32_t base)
{
    assert(scaleCoverFlow);

    ituCoverFlowLoad(&scaleCoverFlow->coverFlow, base);
    ituWidgetSetUpdate(scaleCoverFlow, ituScaleCoverFlowUpdate);
    ituWidgetSetDraw(scaleCoverFlow, ituScaleCoverFlowDraw);
}
