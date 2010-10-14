require 'image_size'
require 'open3'

describe 'image resizer' do
  before(:all) do
    Dir.chdir(File.join(File.dirname(__FILE__), '..')) do
      puts `make 2>&1`
    end
  end

  it "should resize my an image to a portrait" do
    image_data = resize(:file => 'cat-in-hat.jpg', :width => 30, :height => 60)

    dimensions = ImageSize.new(image_data)

    dimensions.width.should == 30
    dimensions.height.should == 60
  end

  it "should resize my an image to a landscape" do
    image_data = resize(:file => 'cat-in-hat.jpg', :width => 600, :height => 200)

    dimensions = ImageSize.new(image_data)

    dimensions.width.should == 600
    dimensions.height.should == 200
  end
end

def resize params
  file = params[:file] || raise("gimme an image")
  image_root = File.join(File.dirname(__FILE__), 'fixtures')
  script_path = File.join(File.dirname(__FILE__), '..', 'image_resizer.cgi')
  padding_color = params[:padding_color] || 'ffffff'

  script_params = ''

  [
          ['REQUEST_METHOD', 'get'],
          ['QUERY_STRING', "width=#{params[:width] || raise("gimme a width")}&height=#{params[:height] || raise("gimme a height")}"],
          ['IMAGE_ROOT', image_root],
          ['PATH_INFO', file],
          ['PADDING_COLOR', padding_color]
  ].each do |key, val|
    script_params += "#{key}='#{val}' "
  end

  had_error = false
  output = ''

  puts "#{script_params} #{script_path}"
  
  Open3.popen3("#{script_params} #{script_path}") do |stdin, stdout, stderr|
    stdout_output, stderr_output = stdout.read, stderr.read

    output += stdout_output unless stdout_output.empty?

    unless stderr_output.empty?
      had_error = true
      output += stderr_output
    end
  end

  raise output if had_error

  output.gsub!(/.*Content-type.+\s+/, '')
  test_image_file = "test_image.#{file.split('.').pop}"
  File.unlink(test_image_file) if File.exists?(test_image_file)
  File.open(test_image_file, 'wb').write(output)
  output
end
