#include <algorithm>
void getBoardInfo();

vector<int> setThresholds(vector<int> input_vector, int comparator);
vector<int> getTriangleLocations(Mat img, int axis);
vector<int> getObjectLocations(Mat img, int axis);
void setLocations(vector<int> input_vector, int comparator);

vector<vector<int>> setXThreshold(vector<int> triangle_vector);
void setPos(Point2f point, vector<vector<int>> threshold_vector);
int setY(int y,  vector<int> input_vector);
int setX(int x, int y, vector<vector<int>> input_vector );
void setXLocations(vector<int> input_vector, vector<int> threshold_vector);
void getBoardInfo()
{
    Mat image;
    Point2f point_cyc;
    int game[7][7];
    newFrame = imread("board3.png");

    Point2f point;
    int trig_x[7];
    int trig_avg = 0;
    int temp = 0;

    vector<int> triangle_x;
    vector<int> object_x;

    triangle_x = getTriangleLocations(newFrame, 'x');
    object_x = getObjectLocations(newFrame, 'x');

    vector<int> vector_deneme = {150, 155, 170, 175, 190, 195};
    vector<int> vector_thresh_deneme = {
        160, 180, 200,
    };

    //setLocations(object_y, 10);

    //setThresholds(triangle_x, 0);

    //setLocations(triangle_x, 0);

    //setXLocations(object_x, triangle_x);
    setXThreshold(triangle_x);
}

vector<int> getObjectLocations(Mat img, int axis)
{
    cout << "cylnders" << endl;
    Mat image;
    Point2f point;
    vector<int> output_vector;
    image = templateMatching(templateExtract(img, 'R'));
    //while(true)
    dispImage(image, "matching", 3);
    Mat imageSelected = Mat::zeros(image.size(), CV_8UC1);
    vector<vector<Point>> contours; // Vector for storing contour
    vector<Vec4i> hierarchy;
    image.convertTo(image, CV_8UC1);
    findContours(image, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
    for (int i = 0; i < contours.size(); i++) // iterate through each contour.
    {
        imageSelected = Mat::zeros(image.size(), CV_8UC1);
        drawContours(imageSelected, contours, i, Scalar(255, 0, 0), CV_FILLED, 8, hierarchy);
        point = findCenter(imageSelected);
        if (axis == 'x')
        {
            cout << point.x << endl;
            output_vector.push_back(point.x);
        }
        else if (axis == 'y')
        {
            cout << point.y << endl;
            output_vector.push_back(point.y);
        }
    }

    sort(output_vector.begin(), output_vector.end());
    return output_vector;
}

vector<int> getTriangleLocations(Mat img, int axis)
{

    vector<int> output_vector;
    Point2f point;
    for (int i = 0; i <= 6; i++)
    {
        point = getBoardSlot(img, i, i);
        if (axis == 'x')
        {
            output_vector.push_back(point.x);
        }
        else if (axis == 'y')
        {
            output_vector.push_back(point.y);
        }
        cout << output_vector[i] << endl;
    }

    sort(output_vector.begin(), output_vector.end());
    return output_vector;
}

vector<int> setThresholds(vector<int> input_vector, int comparator)
{

    sort(input_vector.begin(), input_vector.end());

    vector<int> threshold_vector;

    threshold_vector.push_back(input_vector[0] + comparator);
    for (int i = 1; i < input_vector.size(); i++)
    {
        bool threshold_test = input_vector[i] > (threshold_vector.back() + comparator);
        if (threshold_test)
        {
            threshold_vector.push_back(input_vector[i] + comparator);
        }
    }
    cout << "\nthreshold values:" << endl;

    for (int i = 0; i < threshold_vector.size(); i++)
    {
        cout << *(std::begin(threshold_vector) + i) << endl;
    }
    cout << "done" << endl;

    return threshold_vector;
}

void setLocations(vector<int> input_vector, int comparator)
{

    vector<int> threshold_vector = setThresholds(input_vector, comparator);

    cout << "setYLocations:" << endl;
    vector<int> output_vector;
    for (int i = 0; i < input_vector.size(); i++)
    {
        for (int k = 0; k < (threshold_vector.size()); k++)
        {
            input_vector[i] > threshold_vector[k];
            bool location_test = (threshold_vector[k] < input_vector[i]) && (input_vector[i] < threshold_vector[k + 1]);
            if (location_test)
            {
                output_vector.push_back(6 - k);
                break;
            }
            if (k == threshold_vector.size() - 1)
            {
                output_vector.push_back(6 - k);
            }
        }
    }

    for (int i = 0; i < output_vector.size(); i++)
    {
        cout << *(std::begin(output_vector) + i) << endl;
    }
    cout << "done" << endl;
}

void setXLocations(vector<int> input_vector, vector<int> threshold_vector)
{

    cout << "set X Locations:" << endl;
    vector<int> output_vector;

    for (int i = 0; i < input_vector.size(); i++)
    {
        for (int k = 0; k < (threshold_vector.size()); k++)
        {
            input_vector[i] > threshold_vector[k];
            bool location_test = (threshold_vector[k] < input_vector[i]) && (input_vector[i] < threshold_vector[k + 1]);
            if (location_test)
            {
                output_vector.push_back(k);
                break;
            }
            if (k == threshold_vector.size() - 1)
            {
                output_vector.push_back(k);
            }
        }
    }

    for (int i = 0; i < output_vector.size(); i++)
    {
        cout << *(std::begin(output_vector) + i) << endl;
    }
    cout << "done" << endl;
}

void setPos(Point2f point, vector<vector<int>> threshold_vector, vector<int> y_vector)
{
    int y = setY(point.y, y_vector);
    setX(point.x, y, threshold_vector );
}
int setX(int x, int y, vector<vector<int>> input_vector )
{
    //threshold_vector.at(6 - y);
    vector<int> threshold_vector = input_vector.at(6 - y);
    for (int k = 0; k < (threshold_vector.size()); k++)
    {
        
        bool location_test = (threshold_vector[k] <x) && (x < threshold_vector[k + 1]);
        if (location_test)
        {
            return k;
        }
        if (k == threshold_vector.size() - 1)
        {
            return k;
        }
    }
}

int setY(int y,  vector<int> input_vector)
{
    vector<int> threshold_vector = setThresholds(input_vector, 10);
    for (int k = 0; k < (threshold_vector.size()); k++)
    {
        bool location_test = (threshold_vector[k] < y) && (y < threshold_vector[k + 1]);
        if (location_test)
        {
            return ( threshold_vector.size() - k);
        }
        if (k == threshold_vector.size() - 1)
        {
            return ( threshold_vector.size() - k);
        }
    }
}

vector<vector<int>> setXThreshold(vector<int> triangle_vector)
{
    vector<vector<int>> threshold_vector;

    threshold_vector.push_back(triangle_vector);
    cout << "avg:" << (triangle_vector[0] + triangle_vector[0 + 1]) / 2 << endl;

    int initial_size = triangle_vector.size() - 2;
    for (int k = 0; k < initial_size; k++)
    {
        vector<int> temp_vector;
        for (int i = 0; i < (triangle_vector.size() - 1); i++)
        {
            temp_vector.push_back((triangle_vector[i] + triangle_vector[i + 1]) / 2);
        }

        for (int i = 0; i < temp_vector.size(); i++)
        {
            cout << *(std::begin(temp_vector) + i) << endl;
        }
        cout << "done" << endl;

        threshold_vector.push_back(temp_vector);
        triangle_vector = temp_vector;
    }

    /*
    cout << "setXThreshold" << endl;
    for (int i = 0; i < triangle_vector.size(); i++)
    {
        cout << *(std::begin(triangle_vector) + i) << endl;
    }
    cout << "done" << endl;*/

    return threshold_vector;
}