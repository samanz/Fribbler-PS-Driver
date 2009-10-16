
class LightSources
{
  public:
	LightSources();

	void	add(int row, int col);

	void	setPosition(int row, int col, double orentation);

	int	getValue(double from, double to);
