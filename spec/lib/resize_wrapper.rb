require File.join(File.dirname(__FILE__), 'resized_image')

class ResizeWrapper
  attr_accessor :debug

  def initialize
    Dir.chdir(project_root) do
      exec("make")
    end
  end

  def resize params
    check_resize_params params

    ResizedImage.new(exec("#{script_params(params)} #{File.join(project_root, 'image_resizer.cgi')}"))
  end

  private

  def script_params params
    script_params = ''

    [
            ['REQUEST_METHOD', 'get'],
            ['QUERY_STRING', "width=#{params[:width]}&height=#{params[:height]}"],
            ['IMAGE_ROOT', File.join(File.dirname(__FILE__), '..', 'fixtures')],
            ['PATH_INFO', params[:filename]],
            ['PADDING_COLOR', params[:padding_color]]
    ].each do |key, val|
      script_params += "#{key}='#{val}' "
    end

    return script_params
  end

  def project_root
    File.join(File.dirname(__FILE__), '..', '..')
  end

  def check_resize_params params
    %w{  width height filename padding_color  }.each do |param|
      raise "gimme a #{param}" unless params.has_key?(param.to_sym)
    end
  end

  def exec command
    puts command if debug
    
    output = `#{command} 2>&1`
    raise output if $? != 0;

    puts output if debug

    output
  end
end