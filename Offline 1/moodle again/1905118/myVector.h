#ifndef MY_VECTOR_H
#define MY_VECTOR_H

#define PI (acos(-1.0f))
#define degreeToRadian(x) ((x) * PI / 180.0f)

class myVector
{
private:
    GLfloat x, y, z;

public:
    myVector();
    myVector(GLfloat, GLfloat, GLfloat);
    myVector(const myVector &);

    ~myVector();

    GLfloat getX() const;
    GLfloat getY() const;
    GLfloat getZ() const;

    void setX(GLfloat);
    void setY(GLfloat);
    void setZ(GLfloat);

    GLfloat getLength() const;
    myVector normalize() const;
    GLfloat getDotProduct(const myVector &) const;
    myVector getCrossProduct(const myVector &) const;

    myVector operator+=(const myVector &);
    myVector operator-=(const myVector &);
    myVector operator*=(GLfloat);
    myVector operator/=(GLfloat);

    myVector operator+(const myVector &) const;
    myVector operator-(const myVector &) const;
    myVector operator-() const;
    myVector operator*(GLfloat) const;
    myVector operator/(GLfloat) const;

    myVector rotateAroundAxis(GLfloat, const myVector &);

    friend std::ostream& operator<<(std::ostream&, const myVector&);
};

#endif // MY_VECTOR_H
