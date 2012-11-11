#ifndef IMODELSETTINGSVIEW_H
#define IMODELSETTINGSVIEW_H

class IResponcesSource;

//абстрактный класс для запроса настроек модели
class IModelSettingsView
{
public:

    //получение источника данных
    virtual IResponcesSource* getDataSource() = 0;

    virtual int getInteractionLevel() = 0;

    //запрос настроек у пользователя
    virtual bool askNewSettings() = 0;

    virtual ~IModelSettingsView() {}

    // как получать значения откликов? 0- функция, 1- эксперимент
    virtual int GetModelType()=0;

    virtual int getParal() = 0;
};

#endif // IMODELSETTINGSVIEW_H
