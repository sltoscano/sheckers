using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Media;
using System.Windows.Input;
using System.Windows.Shapes;
using System.Windows.Media.Animation;
using System.Windows.Controls;

namespace sheckersui
{
    public class DragDropHelper
    {
        #region Private Members
        private Point _initialMousePosition;
        private Point _initialSquarePosition;
        private Point _delta;
        private Point _destination;
        private Point _scrollTarget;
        private UIElement _dragSource;
        private UIElement _dropTarget;
        private Rect _dropBoundingBox;
        private bool _mouseCaptured;
        private object _draggedData;
        private object _draggedDataAfterAnimation;

        private Window _topWindow;
        private Canvas _adornerLayer;
        private DragDropAdornerBase _adorner;

        private List<DependencyObject> _hitTestList;

        private static DragDropHelper s_instance;
        private static DragDropHelper Instance
        {
            get
            {
                if (s_instance == null)
                {
                    s_instance = new DragDropHelper();
                }
                return s_instance;
            }
        }
        #endregion

        #region Attached Properties
        public static bool GetIsDragSource(DependencyObject obj)
        {
            return (bool)obj.GetValue(IsDragSourceProperty);
        }
        public static void SetIsDragSource(DependencyObject obj, bool value)
        {
            obj.SetValue(IsDragSourceProperty, value);
        }
        public static UIElement GetDragDropControl(DependencyObject obj)
        {
            return (UIElement)obj.GetValue(DragDropControlProperty);
        }
        public static void SetDragDropControl(DependencyObject obj, UIElement value)
        {
            obj.SetValue(DragDropControlProperty, value);
        }
        public static string GetAdornerLayer(DependencyObject obj)
        {
            return (string)obj.GetValue(AdornerLayerProperty);
        }
        public static void SetAdornerLayer(DependencyObject obj, string value)
        {
            obj.SetValue(AdornerLayerProperty, value);
        }

        public static readonly DependencyProperty IsDragSourceProperty =
            DependencyProperty.RegisterAttached("IsDragSource", typeof(bool),
            typeof(DragDropHelper), new UIPropertyMetadata(false, IsDragSourceChanged));

        public static readonly DependencyProperty DragDropControlProperty =
            DependencyProperty.RegisterAttached("DragDropControl", typeof(UIElement),
            typeof(DragDropHelper), new UIPropertyMetadata(null));

        public static readonly DependencyProperty AdornerLayerProperty =
            DependencyProperty.RegisterAttached("AdornerLayer", typeof(string),
            typeof(DragDropHelper), new UIPropertyMetadata(null));


        private static void IsDragSourceChanged(DependencyObject obj, DependencyPropertyChangedEventArgs e)
        {
            var dragSource = obj as UIElement;
            if (dragSource != null)
            {
                if (Object.Equals(e.NewValue, true))
                {
                    dragSource.PreviewMouseLeftButtonDown += Instance.DragSource_PreviewMouseLeftButtonDown;
                    dragSource.PreviewMouseLeftButtonUp += Instance.DragSource_PreviewMouseLeftButtonUp;
                    dragSource.PreviewMouseMove += Instance.DragSource_PreviewMouseMove;
                }
                else
                {
                    dragSource.PreviewMouseLeftButtonDown -= Instance.DragSource_PreviewMouseLeftButtonDown;
                    dragSource.PreviewMouseLeftButtonUp -= Instance.DragSource_PreviewMouseLeftButtonUp;
                    dragSource.PreviewMouseMove -= Instance.DragSource_PreviewMouseMove;
                }
            }
        }
        #endregion

        #region Utilities
        public static FrameworkElement FindAncestor(Type ancestorType, Visual visual)
        {
            while (visual != null && !ancestorType.IsInstanceOfType(visual))
            {
                visual = (Visual)VisualTreeHelper.GetParent(visual);
            }
            return visual as FrameworkElement;
        }

        public static bool IsMovementBigEnough(Point initialMousePosition, Point currentPosition)
        {
            return (Math.Abs(currentPosition.X - initialMousePosition.X) >= SystemParameters.MinimumHorizontalDragDistance ||
                 Math.Abs(currentPosition.Y - initialMousePosition.Y) >= SystemParameters.MinimumVerticalDragDistance);
        }
        #endregion

        public static void Reset()
        {
            s_instance = null;
        }

        #region Drag Handlers
        private void DragSource_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            try
            {
                Visual visual = e.OriginalSource as Visual;
                _dragSource = e.OriginalSource as UIElement;
                _topWindow = (Window)DragDropHelper.FindAncestor(typeof(Window), visual);
                _initialMousePosition = e.GetPosition(_topWindow);

                string adornerLayerName = GetAdornerLayer(sender as DependencyObject);
                _adornerLayer = (Canvas)_topWindow.FindName(adornerLayerName);

                //not needed since not using the DropTarget property
                //drop targets are figured out programmatically through
                //hit test and if you've set the AllowDrop property on the
                //control.
                //string dropTargetName = GetDropTarget(sender as DependencyObject);
                //_dropTarget = (UIElement)_topWindow.FindName(dropTargetName);

                _draggedData = (sender as FrameworkElement).DataContext;
            }
            catch (Exception exc)
            {
                Console.WriteLine("Exception in DragDropHelper: " + exc.InnerException.ToString());
            }
        }

        // Drag = mouse down + move by a certain amount
        private void DragSource_PreviewMouseMove(object sender, MouseEventArgs e)
        {
            if (!_mouseCaptured && _draggedData != null)
            {
                // Only drag when user moved the mouse by a reasonable amount.
                if (DragDropHelper.IsMovementBigEnough(_initialMousePosition, e.GetPosition(_topWindow)))
                {
                    _adorner = (DragDropAdornerBase)GetDragDropControl(sender as DependencyObject);
                    _adorner.DataContext = _draggedData;
                    _adorner.Opacity = 1.0;
                    // Make sure the piece that is dragged stays the right size
                    _adorner.Width = _dragSource.RenderSize.Width;
                    _adorner.Height = _dragSource.RenderSize.Height;
                    _adorner.Cursor = Cursors.None;
                    _dragSource.Opacity = 0.25;

                    _adornerLayer.Visibility = Visibility.Visible;
                    _adornerLayer.Children.Add(_adorner);
                    _mouseCaptured = Mouse.Capture(_adorner);

                    _initialSquarePosition = _dragSource.TranslatePoint(new Point(0, 0), _topWindow);
                    Canvas.SetLeft(_adorner, _initialSquarePosition.X);
                    Canvas.SetTop(_adorner, _initialSquarePosition.Y);

                    _adornerLayer.PreviewMouseMove += new MouseEventHandler(_adorner_MouseMove);
                    _adornerLayer.PreviewMouseUp += new MouseButtonEventHandler(_adorner_MouseUp);
                }
            }
        }

        private void _adorner_MouseMove(object sender, MouseEventArgs e)
        {
            Point currentPoint = e.GetPosition(_topWindow);

            _delta = new Point(_initialMousePosition.X - currentPoint.X,
                _initialMousePosition.Y - currentPoint.Y);
            _scrollTarget = new Point(_initialSquarePosition.X - _delta.X,
                _initialSquarePosition.Y - _delta.Y);
            
            // Get the piece to actually move over the board
            Canvas.SetLeft(_adorner, _scrollTarget.X);
            Canvas.SetTop(_adorner, _scrollTarget.Y);
            
            _adorner.AdornerDropState = DropState.CannotDrop;

            Point mid = new Point(_adorner.Width / 2, _adorner.Height / 2);
            Point centerOfAdorner = _adorner.TranslatePoint(mid, _topWindow);

            // Setup the target delta for the drop animation
            if (_dropTarget != null)
            {
                mid = new Point(_dropTarget.RenderSize.Width / 2, _dropTarget.RenderSize.Height / 2);
                Point target = _dropTarget.TranslatePoint(mid, _topWindow);
                _destination = new Point(target.X - centerOfAdorner.X,
                    target.Y - centerOfAdorner.Y);
            }

            UIElement element = null;
            _hitTestList = new List<DependencyObject>();
            VisualTreeHelper.HitTest(
                _topWindow, 
                null, 
                CollectAllVisuals_Callback, 
                new PointHitTestParameters(centerOfAdorner));

            foreach (var item in _hitTestList)
            {
                UIElement elem = item as UIElement;
                if (elem != null && elem.AllowDrop)
                {
                    element = elem;
                    break;
                }
            }

            // Only set the drop target if you know for sure that
            // the target allows drops
            if (element != null && element.AllowDrop == true)
            {
                _dropTarget = element;
                GeneralTransform t = _dropTarget.TransformToVisual(_adornerLayer);
                _dropBoundingBox = t.TransformBounds(
                    new Rect(0, 0, _dropTarget.RenderSize.Width, _dropTarget.RenderSize.Height));

                // If inside the droptarget then go ahead and allow the drop
                if (e.GetPosition(_adornerLayer).X > _dropBoundingBox.Left &&
                    e.GetPosition(_adornerLayer).X < _dropBoundingBox.Right &&
                    e.GetPosition(_adornerLayer).Y > _dropBoundingBox.Top &&
                    e.GetPosition(_adornerLayer).Y < _dropBoundingBox.Bottom)
                {
                    _adorner.AdornerDropState = DropState.CanDrop;
                }
            }
            else
            {
                _dropTarget = null;
                _adorner.AdornerDropState = DropState.CannotDrop;
            }
        }

        private HitTestResultBehavior CollectAllVisuals_Callback(HitTestResult result)
        {
            if (result == null || result.VisualHit == null)
                return HitTestResultBehavior.Stop;

            _hitTestList.Add(result.VisualHit);
            return HitTestResultBehavior.Continue;
        }

        private void _adorner_MouseUp(object sender, MouseEventArgs e)
        {
            _dragSource.Opacity = 1.00;

            switch (_adorner.AdornerDropState)
            {
                case DropState.CanDrop:
                    try
                    {
                        
                        Storyboard sb = _adorner.Resources["canDrop"] as Storyboard;
                        DoubleAnimation aniX = sb.Children[0] as DoubleAnimation;
                        aniX.To = _destination.X;
                        DoubleAnimation aniY = sb.Children[1] as DoubleAnimation;
                        aniY.To = _destination.Y;
                        sb.Completed += (s, args) =>
                        {
                            _adornerLayer.Children.Clear();
                            _adornerLayer.Visibility = Visibility.Collapsed;

                            // Invoke a callback after the animation is done
                            // can be used to commit changes for example
                            if (DropAnimationDone != null && _draggedDataAfterAnimation != null)
                                DropAnimationDone(_draggedDataAfterAnimation, new DragDropEventArgs());
                            _draggedDataAfterAnimation = null;
                        };
                        FrameworkElement target = _dropTarget as FrameworkElement;
                        _draggedDataAfterAnimation = target.DataContext;
                        sb.Begin(_adorner);

                        // Invoke the callback so clients can update the data model
                        if (ItemDropped != null && target != null)
                            ItemDropped(target.DataContext, new DragDropEventArgs(_draggedData));
                    }
                    catch (Exception excep) { throw excep; }
                    break;
                case DropState.CannotDrop:
                    try
                    {
                        Storyboard sb = _adorner.Resources["cannotDrop"] as Storyboard;
                        DoubleAnimation aniX = sb.Children[0] as DoubleAnimation;
                        aniX.To = _delta.X;
                        DoubleAnimation aniY = sb.Children[1] as DoubleAnimation;
                        aniY.To = _delta.Y;
                        sb.Completed += (s, args) =>
                        {
                            _adornerLayer.Children.Clear();
                            _adornerLayer.Visibility = Visibility.Collapsed;
                        };
                        sb.Begin(_adorner);
                    }
                    catch (Exception excep) { throw excep; }
                    break;
            }

            _draggedData = null;
            _adornerLayer.PreviewMouseMove -= new MouseEventHandler(_adorner_MouseMove);
            _adornerLayer.PreviewMouseUp -= new MouseButtonEventHandler(_adorner_MouseUp);

            if (_adorner != null)
            {
                _adorner.ReleaseMouseCapture();
            }
            _adorner = null;
            _mouseCaptured = false;
        }

        private void DragSource_PreviewMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            _draggedData = null;
            _mouseCaptured = false;

            if (_adorner != null)
            {
                _adorner.ReleaseMouseCapture();
            }
        }
        #endregion

        #region Events
        public static event EventHandler<DragDropEventArgs> ItemDragStart;
        public static event EventHandler<DragDropEventArgs> ItemDragEnd;
        public static event EventHandler<DragDropEventArgs> ItemDropped;
        public static event EventHandler<DragDropEventArgs> DropAnimationDone;
        #endregion
    }

    public class DragDropEventArgs : EventArgs
    {
        public object Content;
        public DragDropEventArgs() { }
        public DragDropEventArgs(object content)
        {
            Content = content;
        }
    }


}
