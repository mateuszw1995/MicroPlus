#ifndef CPARTICLES
#define CPARTICLES

#ifdef DO_IMPLEMENTACJI

class CParticles : public C3DSurface
{
    public:
  struct Particle
  {
        friend class CParticles;
    bool   active; /* Active (Yes/No) */
    float life;   /* Particle Life   */
    float fade;   /* Fade Speed      */



    float xi;     /* X Direction     */
    float yi;     /* Y Direction     */
    float zi;     /* Z Direction     */

    float xg;     /* X Gravity       */
    float yg;     /* Y Gravity       */
    float zg;     /* Z Gravity       */

    float size;

        CPolygon poly;

        Particle(bool actv, float lif, float fad,
    float xi1,     /* X Direction     */
    float yi1,     /* Y Direction     */
    float zi1,     /* Z Direction     */
    float xg1,     /* X Gravity       */
    float yg1,     /* Y Gravity       */
    float zg1,     /* Z Gravity       */
    float size,
        CPolygon pol) :
        active(actv), life(lif), fade(fad),
        xi(xi1),
        yi(yi1),
        zi(zi1),
        xg(xg1),
        yg(yg1),
        zg(zg1),
        poly(pol)
        {}
  };
  bool visible;
  GLuint unactives;
  CEnum SystemType;
  vector<Particle> Particles;

  void (*ResetFunction)(Particle&);
  GLuint (*MoveFunction)(Particle&);

  void Restart()
  {
        for(GLuint i = 0; i < Particles.size(); ++i)
        {
          ResetFunction(Particles[i]);
        }
  }
  CParticles(GLuint much, CTexture* newtex, CEnum types, void (*ResFunction)(Particle&), GLuint (*MovFunction)(Particle&)) : ResetFunction(ResFunction), MoveFunction(MovFunction), SystemType(types)
  {
        visible = true;
        unactives = 0;
        for(GLuint i = 0; i < much; ++i)
        {
            Particle copy;
            copy.poly = CPolygon(newtex, -3);
            copy.active = true;


            copy.poly(0.01, 0.01, 0.0, NO_COLOR, 1.0, 0.0)
                     (-0.01, 0.01, 0.0, NO_COLOR, 0.0, 0.0)
                     (0.01, -0.01, 0.0, NO_COLOR, 1.0, 1.0)
                     (-0.01, -0.01, 0.0, NO_COLOR, 0.0, 1.0);

       //     copy.poly(0.0, 0.0, 0.0, NO_COLOR, 1.0, 0.0);
            copy.poly.setNormal(0.0, 0.0, -1.0);

            ResetFunction(copy);

            Particles.push_back(copy);

        }

  }
  CParticles(void)
  {
  }

  void Draw(void);

};

void CParticles::Draw(void)
{
    if(visible)
    {
     glTranslatef(x, y, z);
    glRotatef(RotX, 1.0, 0.0, 0.0);
    glRotatef(RotY, 0.0, 1.0, 0.0);
    glRotatef(RotZ, 0.0, 0.0, 1.0);

    for(GLuint i = 0; i < Particles.size(); ++i)
    {
        if(Particles[i].active)
        {
        Particles[i].poly.x += Particles[i].xi;
        Particles[i].poly.y += Particles[i].yi;
        Particles[i].poly.z += Particles[i].zi;


        Particles[i].xi -= Particles[i].xg;
        Particles[i].yi -= Particles[i].yg;
        Particles[i].zi -= Particles[i].zg;

         Particles[i].life -= Particles[i].fade;

        for(GLuint k = 0; k < 4; ++k)
         Particles[k].poly.Vertices[k].color.alpha = Particles[k].life;

       MoveFunction(Particles[i]);
        if(Particles[i].life <= 0.0)
        {
            Particles[i].active = false;
            if(SystemType == STILL)
            ResetFunction(Particles[i]);
            else if(SystemType == EXPLOSION)
            {

            }
        }
        Particles[i].poly.Draw();
        }
    }


    glRotatef(-RotZ, 0.0, 0.0, 1.0);
    glRotatef(-RotY, 0.0, 1.0, 0.0);
    glRotatef(-RotX, 1.0, 0.0, 0.0);

 glTranslatef(-x, -y, -z);
}
}
#endif

#endif
